#include "macsublayer.h"
#include <utils/random.h>

MACSublayer::MACSublayer(
  IMACSublayer::Address address,
  IPhysicalConnectionPtr connection,
  QObject *parent) :
  QObject(parent),
  m_address(address),
  m_connection(connection),
  m_reader(this, connection)
{
  m_reader.start();
}

MACSublayer::~MACSublayer()
{
  m_reader.stop();
  clearReadBuffer();
}

void MACSublayer::clearReadBuffer()
{
  while (!m_readBuffer.isEmpty())
  {
    MACFrame frame = m_readBuffer.first();
    delete [] frame.m_data;
    m_readBuffer.removeFirst();
  }
}

bool MACSublayer::flushBytes(BytePtr bytes, uint len)
{
  MLOG("Flushing byte sequence.");
  bool successful = true;
  for (auto it : {0, 1, 1, 1, 1, 1, 1, 0})
    successful &= m_connection->write(it);
  uint counter = 0;
  for (uint j = 0; j < len; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      Bit bit = (bytes.get()[j] >> i) & 1;
      successful &= m_connection->write(bit);
      if (bit == 1)
      {
        counter++;
        if (counter == 5)
        {
          successful &= m_connection->write(0);
          counter = 0;
        }
      }
      else
      {
        counter = 0;
      }
    }
  }
  for (auto it : {0, 1, 1, 1, 1, 1, 1, 0})
    successful &= m_connection->write(it);
  return successful;
}

uint MACSublayer::formFrame(MACFrame *frame, BytePtr &bytes)
{
  // preamble, destination address, source address, length
  uint len = 1 + sizeof(MACFrame) + frame->m_length + 2;
  if (len < MAC_FRAME_SIZE_MIN)
  {
    len = MAC_FRAME_SIZE_MIN;
  }
  Byte *pointer = new Byte[len];
  bytes = BytePtr(pointer, sharedArrayDeleter<Byte>);

  uint index = 0;
  pointer[index++] = 0xaa;

  for (uint i = 0; i < sizeof(MACFrame); i++)
  {
    pointer[index++] = ((Byte *) frame)[i];
  }
  index -= sizeof(frame->m_data);
  for (uint i = 0; i < frame->m_length; i++)
  {
    pointer[index++] = frame->m_data[i];
  }
  while (index < len - 2)
  {
    pointer[index++] = 0;               // Pad field.
  }
  ushort checkSum = qChecksum((const char *) pointer, len - 2);
  pointer[index++] = ((Byte *) &checkSum)[0];
  pointer[index++] = ((Byte *) &checkSum)[1];

  MLOG("Creating frame"
       << "data length =" << frame->m_length
       << "total length =" << len
       << "checkSum =" << checkSum);

  return len;
}

bool MACSublayer::sendFrame(MACFrame *frame)
{
  BytePtr bytes;
  uint len = formFrame(frame, bytes);

  for (int counter = 0; counter < 16; counter++)
  {
    // Wait for a moment, when nobody writes to medium.
    while (!m_connection->isFree(10))
    {
      m_connection->wait(10);
    }
    // Now try to write.
    if (flushBytes(bytes, len))
    {
      return true;
    }
    // Collision occured. Wait for a random period.
    uint waitPeriod;
    if (counter > 10)
      waitPeriod = random(1 << 10);
    else
      waitPeriod = random(1 << counter);
    MLOG("Collision occured"
         << "counter =" << counter
         << "waitPeriod =" << waitPeriod);
    m_connection->wait(waitPeriod);
  }

  return false;
}

bool MACSublayer::saveFrame(MACFrame frame)
{
  QMutexLocker locker(&m_readBufferMutex);
  m_readBuffer.append(frame);
  m_readBufferWaitCondition.wakeOne();
  MLOG("Frame saved to read buffer."
       << "from =" << frame.m_sourceAddress
       << "to =" << frame.m_destinationAddress
       << "length =" << frame.m_length);
  return true;
}

bool MACSublayer::save(Byte *bytes, uint len)
{
  uint index = 0;
  if (len < MAC_FRAME_SIZE_MIN ||
      len > MAC_FRAME_DATA_SIZE_MAX ||
      bytes[index++] != 0xaa)
  {
    // Damaged frame.
    return false;
  }

  ushort checkSum = qChecksum((const char *) bytes, len - 2);
  if (bytes[len - 2] != ((Byte *) &checkSum)[0] ||
      bytes[len - 1] != ((Byte *) &checkSum)[1])
  {
    return false;
  }

  MACFrame frame;
  for (Byte *p = (Byte *) &frame;
       index <= sizeof(MACFrame) - sizeof(frame.m_data); p++)
  {
    *p = bytes[index++];
  }
  if (frame.m_destinationAddress != MAC_BROADCAST_ADDRESS &&
      frame.m_destinationAddress != m_address)
  {
    return false;
  }
  frame.m_data = new Byte[frame.m_length];
  for (uint i = 0; i < frame.m_length; i++)
  {
    frame.m_data[i] = bytes[index++];
  }
  return saveFrame(frame);
}

bool MACSublayer::save(BitList bits)
{
  if (bits.size() & 0x7)
  {
    return false;
  }
  uint len = bits.size() >> 3;
  Byte *bytes = new Byte[len];
  Byte byte = 0;
  int i = 0;
  int j = 0;
  for (auto bit : bits)
  {
    byte |= bit << j;
    j++;
    if (j == 8)
    {
      bytes[i++] = byte;
      byte = 0;
      j = 0;
    }
  }
  bool success = save(bytes, len);
  delete [] bytes;
  return success;
}

bool MACSublayer::write(
  IMACSublayer::Address address, Byte *bytes, uint len)
{
  QMutexLocker locker(&m_writeMethodMutex);
  MACFrame frame(address, m_address, len, bytes);
  return sendFrame(&frame);
}

uint MACSublayer::read(
  IMACSublayer::Address &address, BytePtr &bytes, ulong time)
{
  QMutexLocker locker(&m_readBufferMutex);
  while (m_readBuffer.isEmpty())
  {
    MLOG("No frames in read buffer: waiting for" << time);
    if (!m_readBufferWaitCondition.wait(&m_readBufferMutex, time))
    {
      return 0;
    }
  }
  MACFrame frame = m_readBuffer.first();
  bytes = BytePtr(frame.m_data, sharedArrayDeleter<Byte>);
  address = frame.m_sourceAddress;
  m_readBuffer.removeFirst();
  if (address == m_address)
  {
    // If it is my frame.
    return 0;
  }
  return frame.m_length;
}

void MACSublayer::reconnect()
{
  m_reader.stop();
  QMutexLocker locker1(&m_writeMethodMutex);
  QMutexLocker locker2(&m_readBufferMutex);
  clearReadBuffer();
  m_connection->reconnect();
  m_reader.start();
}

MACSublayer::Address MACSublayer::getAddress()
{
  return m_address;
}
