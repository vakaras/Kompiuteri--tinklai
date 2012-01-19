#include "socket.h"
#include <QDateTime>
#include <utils/sharedarraydeleter.h>

Socket::Socket(INetworkLayer *network,
               ITransportLayer::Address destinationAddress,
               ITransportLayer::Port destinationPort,
               ITransportLayer::Port sourcePort,
               Type socketType,
               QObject *parent) :
  QObject(parent),
  m_socketType(socketType),
  m_destinationAddress(destinationAddress),
  m_destinationPort(destinationPort),
  m_sourcePort(sourcePort),
  m_connected(false),
  m_network(network),
  m_rtt(500),
  m_destinationWindowSize(0),
  m_ackSequence(0),
  m_congestionWindowSize(1),
  m_threshold(MAX_SEGMENT_AMOUNT)
{
  m_sourceSequence = (uint) QDateTime::currentMSecsSinceEpoch();
}

bool Socket::send(TCPPacket packet)
{
  uint windowSize = MAX_BUFFER_SIZE - m_readBuffer.size();
  packet.m_windowSize = windowSize < (MAX_BUFFER_SIZE >> 1) ? 0 : windowSize;
  qDebug() << "Send window size:" << packet.m_windowSize;
  BytePtr bytes;
  uint len = packet.toBytes(bytes);
  return m_network->send(m_destinationAddress, bytes.get(), len);
}

bool Socket::sendBurst(Byte *bytes, uint len)
{
  bool allOk = true;
  for (uint i = 0; i < len; i += MAX_SEGMENT_SIZE)
  {
    TCPPacket packet;

    packet.m_sourcePort = m_sourcePort;
    packet.m_destinationPort = m_destinationPort;
    packet.m_sequenceNumber = m_sourceSequence;
    packet.m_ackFlag = 1;
    packet.m_ackNumber = m_destinationSequence;
    packet.m_dataLength = qMin(len - i, MAX_SEGMENT_SIZE);
    packet.m_data = bytes + i;

    m_sourceSequence += packet.m_dataLength;

    allOk &= send(packet);
  }
  return allOk;
}

bool Socket::send(Byte *bytes, uint len)
{
  if (!m_connected)
  {
    return false;
  }
  QMutexLocker locker2(&m_sendMutex);
  QMutexLocker locker(&m_socketMutex);

  m_ackSequence = m_sourceSequence;

  for (uint i = 0; i < len;)
  {
    qDebug() << "Sending 1.";
    uint oldAckSequence = m_ackSequence;
    if (m_destinationWindowSize)
    {
      // We are allowed to send data.
      Byte* start = bytes + i;
      uint length = qMin(qMin(len - i, m_destinationWindowSize),
                         m_congestionWindowSize * MAX_SEGMENT_SIZE);
      sendBurst(start, length);

      MSec now = QDateTime::currentMSecsSinceEpoch();
      m_senderTimeoutMoment = now + (m_rtt << 1);
      while (m_senderTimeoutMoment > now &&
             m_ackSequence < m_sourceSequence)
      {
        m_senderWaitCondition.wait(&m_socketMutex,
                                   m_senderTimeoutMoment - now);
        now = QDateTime::currentMSecsSinceEpoch();
      }
      if (m_ackSequence != m_sourceSequence)
      {
        // Timeout occured.
        qDebug() << "Timeout" << m_rtt;
        m_sourceSequence = m_ackSequence;
        if (m_congestionWindowSize > 1)
          m_threshold = m_congestionWindowSize >> 1;
        else
          m_threshold = 1;
        m_congestionWindowSize = 1;
        m_rtt += 200;
      }
      else
      {
        if (m_congestionWindowSize >= m_threshold)
        {
          m_congestionWindowSize++;
        }
        else
        {
          m_congestionWindowSize <<= 1;
        }
        m_rtt = ((m_rtt * 7) >> 3) + ((now - m_senderTimeoutMoment) >> 3);
      }
    }
    else
    {
      qDebug() << "Blocking for window." << m_destinationWindowSize;
      // We are not allowed to send data. Wait.
      m_senderWaitCondition.wait(&m_socketMutex);
    }
    i += m_ackSequence - oldAckSequence;
  }
  qDebug() << "----->Sent.";
  return true;
}

uint Socket::receive(BytePtr &bytes, ulong time)
{
  QMutexLocker locker(&m_socketMutex);
  while (m_destinationSequence <= m_readBufferLowerBound)
  {
    qDebug() << "Waiting." << m_destinationSequence << m_readBufferLowerBound;
    if (!m_readBufferWaitCondition.wait(&m_socketMutex, time))
    {
      return 0;
    }
  }
  qDebug() << "Copying." << m_readBuffer.size();
  uint len = m_destinationSequence - m_readBufferLowerBound + 1;
  Byte *data = new Byte[len];
  bytes = BytePtr(data, sharedArrayDeleter<Byte>);
  for (uint i = m_readBufferLowerBound; i <= m_destinationSequence; i++)
  {
    data[i - m_readBufferLowerBound] = m_readBuffer[i];
    m_readBuffer.remove(i);
  }
  m_readBufferLowerBound = m_destinationSequence + 1;
  return len;
}

void Socket::disconnect()
{
  // TODO
}

bool Socket::isConnected()
{
  QMutexLocker locker(&m_socketMutex);
  return m_connected;
}

bool Socket::connect()
{
  qDebug() << "Socket connecting.";
  QMutexLocker locker(&m_socketMutex);

  TCPPacket packet;
  packet.m_sourcePort = m_sourcePort;
  packet.m_destinationPort = m_destinationPort;
  packet.m_sequenceNumber = m_sourceSequence++;
  packet.m_synFlag = 1;
  packet.m_ackFlag = 0;
  send(packet);
  m_connectWaitCondition.wait(&m_socketMutex, 1000);
  return m_connected;
}

void Socket::sendConnectResponse()
{
  QMutexLocker locker(&m_socketMutex);
  qDebug() << "Socket: Sending response.";
  TCPPacket packet;
  packet.m_sourcePort = m_sourcePort;
  packet.m_destinationPort = m_destinationPort;
  packet.m_sequenceNumber = m_sourceSequence++;
  packet.m_ackNumber = ++m_destinationSequence;
  packet.m_synFlag = 1;
  packet.m_ackFlag = 1;
  send(packet);
  m_connectWaitCondition.wait(&m_socketMutex, 1000);
}

void Socket::parseSegment(ITransportLayer::Address address,
                          TCPPacket packet)
{
  QMutexLocker locker(&m_socketMutex);
  qDebug() << "Socket: Parsing segment.";
  qDebug() << packet.m_ackNumber << m_sourceSequence;
  if (address != m_destinationAddress ||
      !packet.m_ackFlag ||
      (packet.m_synFlag && m_connected) ||
      (!packet.m_synFlag && !m_connected))
  {
    // Damaged packet.
    qDebug() << "Damaged packet.";
  }
  else
  {
    if (packet.m_synFlag)
    {
      if (packet.m_ackNumber == m_sourceSequence)
      {
        finalizeConnecting(packet);
      }
    }
    else
    {
      m_ackSequence = packet.m_ackNumber;
      qDebug() << "ACK received:" << this << m_ackSequence;
      uint len = packet.m_dataLength;
      uint sequence = packet.m_sequenceNumber;
      if (sequence + len <= m_destinationSequence)
      {
        qDebug() << "Byte sequence number < lower buffer bound.";
      }
      else if (sequence >= m_readBufferLowerBound + MAX_BUFFER_SIZE)
      {
        qDebug() << "Byte sequence number > higher buffer bound.";
      }
      else
      {
        qDebug() << "Copying." << this << m_destinationSequence << sequence;
        for (uint i = qMax(sequence, m_destinationSequence);
             i < qMin(sequence + len,
                      m_readBufferLowerBound + MAX_BUFFER_SIZE);
             i++)
        {
          m_readBuffer[i] = packet.m_data[i-sequence];
        }
        for (uint i = m_readBufferLowerBound;
             i < m_readBufferLowerBound + MAX_BUFFER_SIZE;
             i++)
        {
          if (m_readBuffer.contains(i))
          {
            m_destinationSequence = i;
          }
          else
          {
            break;
          }
        }
        qDebug() << "WAKE UP!" << m_destinationSequence
                 << m_readBufferLowerBound;
        m_readBufferWaitCondition.wakeOne();
      }
      if (m_sendMutex.tryLock())
      {
        // There is no packages for piggybacking. Send ACK alone.
        TCPPacket packet;
        packet.m_sourcePort = m_sourcePort;
        packet.m_destinationPort = m_destinationPort;
        packet.m_sequenceNumber = m_sourceSequence++;
        packet.m_ackFlag = 1;
        packet.m_ackNumber = m_destinationSequence+1;
        send(packet);
        qDebug() << "ACK Sent" << this << packet.m_ackNumber;
        m_sendMutex.unlock();
      }
    }
  }
}

void Socket::finalizeConnecting(TCPPacket packet)
{
  if (m_socketType == Type::Client)
  {
    qDebug() << "Finalize Client.";
    m_destinationSequence = packet.m_sequenceNumber;
    TCPPacket packet;
    packet.m_sourcePort = m_sourcePort;
    packet.m_destinationPort = m_destinationPort;
    packet.m_sequenceNumber = m_sourceSequence++;
    packet.m_synFlag = 1;
    packet.m_ackFlag = 1;
    packet.m_ackNumber = ++m_destinationSequence;
    send(packet);
  }
  else
  {
    qDebug() << "Finalize Server.";
  }
  m_destinationWindowSize = packet.m_windowSize;
  m_readBufferLowerBound = m_destinationSequence;
  m_connected = true;
  m_connectWaitCondition.wakeOne();
}

void Socket::setDestinationSequence(uint sequence)
{
  QMutexLocker locker(&m_socketMutex);
  m_destinationSequence = sequence;
}
