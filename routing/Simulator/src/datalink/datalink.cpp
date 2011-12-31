#include "datalink.h"
#define MODIFY_FRAME_QUEUE(action) \
      m_link->m_frameQueueMutex.lock(); \
      m_link->m_frameQueue. action ;\
      m_link->m_frameQueueMutex.unlock(); \
      m_link->m_frameQueueWaitCondition.wakeOne();

uint inc(uint &value)
{
  uint oldValue = value++;
  if (value == 8)
    value = 0;
  return oldValue;
}

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
  m_writer.start();
}

DataLink::~DataLink()
{
  m_reader.stop();
  m_reader.wait();
  m_writer.stop();
  m_writer.wait();
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
  qDebug() << "Parsing frame!" << thread();
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
  //qDebug("Checksum: %x", checkSum);

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
      byte = 0;
      j = 0;
    }
  }
  if (qChecksum((char *)receivedBuffer, receivedBufferLen) == checkSum)
  {
    // Frame is ok.
    p[0] = receivedBuffer[0];
    p[1] = receivedBuffer[1];
    //qDebug("Header: %x%x", p[1], p[0]);

    saveFrame(&header, receivedBuffer + 2, receivedBufferLen - 2);
  }
//qDebug("Checksum counted: %x",
//       qChecksum((char *)receivedBuffer, receivedBufferLen));

  delete [] receivedBuffer;
}

void DataLink::saveFrame(
  const FrameHeader *header, const Byte *bytes, uint len)
{
  if (header->informationControl.frameType == INFORMATION_FRAME)
  {
    qDebug() << "Trying to save frame.";
    m_lastSequenceNumberReceivedMutex.lock();
    if (header->informationControl.seq !=
        (m_lastSequenceNumberReceived + 1) % 8)
    {
      qDebug() << "Wrong sequence number." << header->informationControl.seq;
      qDebug() << "Expected" << (m_lastSequenceNumberReceived + 1) % 8;
      // TODO: Send reject.
    }
    else
    {
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

      m_lastSequenceNumberReceived = header->informationControl.seq;
      m_acknowledgeFrameNeeded = true;

      setWindowLowerBound(header->informationControl.next);
      qDebug() << "Writer notified" << m_lastSequenceNumberReceived;
    }
    m_lastSequenceNumberReceivedMutex.unlock();
  }
  else if (header->supervisoryControl.zero == SUPERVISORY_FRAME)
  {
    if (header->supervisoryControl.type == RECEIVE_READY)
    {
      setWindowLowerBound(header->supervisoryControl.next);
    }
    else
    {
      qDebug() << "Uknown supervisory frame type.";
    }
  }
  else
  {
    qDebug() << "Got unknown frame!";
  }
}

void DataLinkReader::run()
{
  qDebug() << "Start";
  State state = Search;
  m_link->m_lastSequenceNumberReceivedMutex.lock();
  m_link->m_lastSequenceNumberReceived = 7;
  m_link->m_acknowledgeFrameNeeded = false;
  m_link->m_lastSequenceNumberReceivedMutex.unlock();
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

  m_packetSent = true;

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
  return m_packetSent;
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

  QMutexLocker locker(&m_link->m_writerMutex);
  // TODO: Send U-Frame SNRM.
  m_link->m_writeWindowLowerBound = 0;
  m_link->m_writeWindowUpperBound = 0;
  QList<DataLink::Frame> sendList;

  while (m_go)
  {
    qDebug() << "Writer!" << sendList.size() << m_link->m_frameQueue.size();
    if (!m_link->m_packetSent)
    {
      // An error occured. Cancel all operations.
      sendList.clear();
      MODIFY_FRAME_QUEUE(clear());
    }
    while (!sendList.isEmpty())
    {
      qDebug() << "WORKS" << m_link->m_writeWindowLowerBound;
      if (m_link->m_writeWindowLowerBound ==
          sendList.first().header.informationControl.seq)
        break;
      sendList.removeFirst();
      MODIFY_FRAME_QUEUE(removeFirst());
    }
    while (sendList.size() < m_link->m_frameQueue.size())
    {
      if (sendList.size() >= 3)
        break;
      sendList.append(m_link->m_frameQueue.at(sendList.size()));
      sendList.last().header.informationControl.seq =
          inc(m_link->m_writeWindowUpperBound);
    }
    QDateTime now = QDateTime::currentDateTime();
    m_link->m_lastSequenceNumberReceivedMutex.lock();
    if (m_link->m_acknowledgeFrameNeeded &&
        (sendList.isEmpty() ||
        (!sendList.first().time.isNull() &&
        sendList.first().time.time().addMSecs(20) > now.time())))
    {
      // No information frames for piggybacking.
      DataLink::FrameHeader header;
      header.supervisoryControl.frameType = 1;
      header.supervisoryControl.zero = 0;
      header.supervisoryControl.type = RECEIVE_READY;
      header.supervisoryControl.next =
          (m_link->m_lastSequenceNumberReceived + 1) % 8;
      qDebug() << "Supervisory next: " << header.supervisoryControl.next;
      m_link->writeFrame(&header, NULL, 0);
      m_link->m_acknowledgeFrameNeeded = false;
    }
    for (auto it : sendList)
    {
      if (!it.time.isNull() && it.time.time().addMSecs(20) > now.time())
        break;
      it.header.supervisoryControl.next =
          m_link->m_lastSequenceNumberReceived + 1;
      m_link->writeFrame(&it.header, it.start, it.len);
      it.counter++;
      it.time = QDateTime::currentDateTime();
      m_link->m_acknowledgeFrameNeeded = false;
    }
    m_link->m_lastSequenceNumberReceivedMutex.unlock();

    m_link->m_writerWaitCondition.wait(&m_link->m_writerMutex, 2000);
  }

}

void DataLink::setWindowLowerBound(uint value)
{
  m_writerMutex.lock();
  m_writeWindowLowerBound = value;
  qDebug() << "Updated lower bound:" << value;
  m_writerMutex.unlock();
  m_writerWaitCondition.wakeOne();
}
