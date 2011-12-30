#include "datalink.h"

DataLink::DataLink(IPhysicalConnection *connectionIn,
                   IPhysicalConnection *connectionOut,
                   uint maxFrameSize,
                   uint maxFrameCount,
                   QObject *parent) :
  QObject(parent),
  m_connectionIn(connectionIn),
  m_connectionOut(connectionOut),
  m_maxFrameSize(maxFrameSize),
  m_reader(this),
  m_maxReceivedDataBufferSize(maxFrameSize * maxFrameCount),
  m_writer(this)
{
  connectionIn->reset();
  connectionOut->reset();
  m_reader.start();
}

DataLink::~DataLink()
{
  m_reader.stop();
  m_reader.wait();
}

void DataLink::writeFrame(
  const FrameHeader *header, const Byte *bytes, uint len)
{
  Byte *buffer = new Byte[2 + len + 2];
  // header data checksum
  buffer[0] = ((Byte *) header)[0];
  buffer[1] = ((Byte *) header)[1];
  for (uint i = 0; i < len; i++)
  {
    buffer[i+2] = bytes[i];
  }
  ushort checkSum = qChecksum((const char *)buffer, len + 2);
  buffer[2+len] = checkSum & 0xff;
  buffer[3+len] = (checkSum >> 8) & 0xff;

  for (auto it : {0, 1, 1, 1, 1, 1, 1, 0})
    m_connectionOut->write(it);
  uint counter = 0;
  for (uint j = 0; j < len + 4; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      Bit bit = (buffer[j] >> i) & 1;
      m_connectionOut->write(bit);
      if (bit == 1)
      {
        counter++;
        if (counter == 5)
        {
          m_connectionOut->write(0);
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
    m_connectionOut->write(it);
  delete [] buffer;
}

void DataLink::parseFrame(QList<Bit> *buffer)
{
  qDebug() << "Parsing frame!";
  uint len = buffer->size();
  if ((len < 32) || (len & 0x7))
  {
    // Frame is damaged.
    return;
  }
  FrameHeader header;
  Byte *p = (Byte*) &header;
  *p = 0;

  ushort checkSum = 0;
  for (int i = 15; i >= 0; i--)
  {
    checkSum |= buffer->last() << i;
    buffer->removeLast();
  }
  qDebug("Checksum: %x", checkSum);

  Byte *receivedBuffer = new Byte[(len >> 3) - 2];
  int receivedBufferLen = 0;
  int j = 0;
  Byte byte = 0;
  for (auto bit : *buffer)
  {
    byte |= bit << j;
    j++;
    if (j == 8)
    {
      receivedBuffer[receivedBufferLen++] = byte;
      qDebug("%x", byte);
      byte = 0;
      j = 0;
    }
  }
  if (qChecksum((char *)receivedBuffer, receivedBufferLen) == checkSum)
  {
    // Frame is ok.
    p[0] = receivedBuffer[0];
    p[1] = receivedBuffer[1];
    qDebug("Header: %x%x", p[1], p[0]);

    saveFrame(&header, receivedBuffer + 2, receivedBufferLen - 2);
  }
  qDebug("Checksum counted: %x", qChecksum((char *)receivedBuffer, receivedBufferLen));

  delete [] receivedBuffer;
}

void DataLink::saveFrame(
  const FrameHeader *header, const Byte *bytes, uint len)
{
  if (header->informationControl.frameType == INFORMATION_FRAME)
  {
    // TODO: Check and change window.
    // TODO: Mark that response is needed.
    m_receivedDataBufferMutex.lock();
    if (m_receivedDataBuffer.size() + len <= m_maxReceivedDataBufferSize)
    {
      for (uint i = 0; i < len; i++)
      {
        m_receivedDataBuffer.append(bytes[i]);
      }
    }
    m_receivedDataBufferMutex.unlock();
    m_receivedDataBufferWaitCondition.wakeOne();
  }
}

void DataLinkReader::run()
{
  qDebug() << "Start";
  State state = Search;
  QList<Bit> buffer;
  uint count[2];
  count[1] = 0;                         // How many 1 found.
  count[0] = 0;                         // How many 0 found.
  Bit lastBit = 0;
  while (m_go)
  {
    bool timedOut = false;
    Bit bit = m_link->m_connectionIn->read(100, &timedOut);
    if (timedOut)
    {
      continue;
    }
    qDebug() << "Bit: " << this << bit;
    buffer.append(bit);
    if (lastBit == bit)
    {
      count[bit]++;
    }
    else
    {
      if (bit == 0)
      {
        if (count[1] == 5)
        {
          // Removing stuffed bit.
          buffer.pop_back();
        }
        else if (count[1] == 6)
        {
          qDebug() << "Found marker";
          // We have found marker: 0111 1110.
          if (state == Search)
          {
            qDebug() << "Clear buffer. New frame begins.";
            // Frame begins.
            state = Read;
            buffer.clear();
          }
          else                          // state == Read
          {
            // One frame ends, other begins.
            for (int i = 0; i < 8; i++)
            {
              // Remove marker byte.
              buffer.pop_back();
            }
            m_link->parseFrame(&buffer);
            state = Read;
            buffer.clear();
          }
        }
        else if (count[1] > 6)
        {
          // Damaged frame.
          state = Search;
        }
      }
      count[bit] = 1;
    }

    lastBit = bit;
  }
}

bool DataLink::write(const Byte *bytes, uint len)
{
  QMutexLocker locker(&m_writeFunctionMutex);
  QMutexLocker locker2(&m_frameQueueMutex);

  for (uint i = 0; i < len; i += m_maxFrameSize)
  {
    if (i + m_maxFrameSize < len)
      m_frameQueue.enqueue(Frame(bytes + i, m_maxFrameSize));
    else
      m_frameQueue.enqueue(Frame(bytes + i, len - i));
  }

  while (!m_frameQueue.isEmpty())
  {
    m_frameQueueWaitCondition.wait(&m_frameQueueMutex);
  }
  return true;
}

int DataLink::read(Byte *bytes, uint maxlen, ulong time)
{
  QMutexLocker locker(&m_receivedDataBufferMutex);
  while (m_receivedDataBuffer.size() == 0)
  {
    if (!m_receivedDataBufferWaitCondition.wait(
          &m_receivedDataBufferMutex, time))
    {
      return 0;
    }
  }
  int counter = 0;
  while (!m_receivedDataBuffer.isEmpty() && maxlen > 0)
  {
    bytes[counter++] = m_receivedDataBuffer.first();
    maxlen--;
    m_receivedDataBuffer.removeFirst();
  }
  return counter;
}

void DataLink::reset()
{
  QMutexLocker locker(&m_receivedDataBufferMutex);
  m_reader.stop();
  m_reader.wait();
  m_connectionIn->reset();
  m_connectionOut->reset();
  m_receivedDataBuffer.clear();
  m_reader.start();
  // TODO: Išvalyti „rašytojo“ giją.
}

void DataLinkWriter::run()
{
}
