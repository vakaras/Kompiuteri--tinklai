#include "socket.h"
#include <QDateTime>

Socket::Socket(INetworkLayer *network,
               ITransportLayer::Address destinationAddress,
               ITransportLayer::Port destinationPort,
               ITransportLayer::Port sourcePort,
               QObject *parent) :
  QObject(parent),
  m_destinationAddress(destinationAddress),
  m_destinationPort(destinationPort),
  m_sourcePort(sourcePort),
  m_connected(false),
  m_network(network)
{
}

bool Socket::send(TCPPacket packet)
{
  BytePtr bytes;
  uint len = packet.toBytes(bytes);
  return m_network->send(m_destinationAddress, bytes.get(), len);
}

bool Socket::send(Byte *bytes, uint len)
{
  // TODO
  return false;
}

uint Socket::receive(BytePtr &bytes, ulong time)
{
  // TODO
  return 0;
}

void Socket::disconnect()
{
  // TODO
}

bool Socket::isConnected()
{
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
  return isConnected();
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
}

void Socket::parseSegment(ITransportLayer::Address address,
                          TCPPacket packet)
{
  QMutexLocker locker(&m_socketMutex);
  qDebug() << "Socket: Parsing segment.";
  if (address != m_destinationAddress ||
      !packet.m_ackFlag ||
      packet.m_ackNumber != m_sourceSequence ||
      (packet.m_synFlag && m_connected) ||
      (!packet.m_synFlag && !m_connected))
  {
    // Damaged packet.
  }
  else
  {
    if (packet.m_synFlag)
    {
      finalizeConnecting();
    }
    else
    {
      // TODO: Data arrived.
    }
  }
}

void Socket::finalizeConnecting()
{
  TCPPacket packet;
  packet.m_sourcePort = m_sourcePort;
  packet.m_destinationPort = m_destinationPort;
  packet.m_sequenceNumber = m_sourceSequence++;
  packet.m_ackFlag = 1;
  send(packet);
  m_connected = true;
}

void Socket::setDestinationSequence(uint sequence)
{
  QMutexLocker locker(&m_socketMutex);
  m_destinationSequence = sequence;
}

void Socket::setConnected(bool connected)
{
  QMutexLocker locker(&m_socketMutex);
  m_connected = connected;
}
