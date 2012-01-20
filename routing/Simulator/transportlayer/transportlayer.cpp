#include "transportlayer.h"

TransportLayer::TransportLayer(INetworkLayer *network, QObject *parent) :
  QObject(parent), m_network(network), m_reader(network, this),
  m_lastPort(1000)
{
  m_reader.start();
}

TransportLayer::~TransportLayer()
{
  m_reader.stop();
}

void TransportLayer::parseSegment(Address address, BytePtr bytes, uint len)
{
  if (len >= TCPPacket::headerLength())
  {
    TCPPacket packet = TCPPacket::fromBytes(bytes);
    if (packet.length() == len)
    {
      if (packet.m_synFlag && !packet.m_ackFlag)
      {
        // Client tries connecting to server. (First handshake.)
        QMutexLocker locker(&m_listenersMapMutex);
        if (m_listenersMap.contains(packet.m_destinationPort))
        {
          m_listenersMap[packet.m_destinationPort]->addConnectionInfo(
                address, packet.m_sourcePort, packet.m_sequenceNumber);
        }
      }
      else
      {
        QMutexLocker locker(&m_socketsMapMutex);
        ushort sourcePort = packet.m_sourcePort;
        ushort destinationPort = packet.m_destinationPort;
        ConnectionId connectionId = ConnectionId(address, sourcePort,
                                                 destinationPort);
        if (m_socketsMap.contains(connectionId))
        {
          m_socketsMap[connectionId]->parseSegment(address, packet);
        }
      }
    }
  }
}

TransportLayer::Port TransportLayer::getFreePort()
{
  QMutexLocker locker(&m_portSetMutex);
  m_lastPort++;
  while (m_portSet.contains(m_lastPort))
  {
    m_lastPort++;
  }
  m_portSet.insert(m_lastPort);
  return m_lastPort;
}

void TransportLayer::freePort(Port port)
{
  QMutexLocker locker(&m_portSetMutex);
  if (m_portSet.contains(port))
  {
    m_portSet.remove(port);
  }
}

IListener* TransportLayer::createListener(Port port)
{
  QMutexLocker locker(&m_portSetMutex);
  QMutexLocker locker2(&m_listenersMapMutex);

  if (m_portSet.contains(port))
  {
    return NULL;
  }

  m_portSet.insert(port);
  ListenerPtr listener(new Listener(this, port));
  m_listenersMap[port] = listener;

  return listener.get();
}

ISocket* TransportLayer::connect(Address address, Port port)
{
  qDebug() << "Connecting.";
  Port sourcePort = getFreePort();
  qDebug() << "Free port:" << sourcePort;
  SocketPtr socket(new Socket(m_network, address, port, sourcePort,
                              Socket::Type::Client));

  qDebug() << "Connecting. 1";
  ConnectionId connectionId = ConnectionId(address, port, sourcePort);
  m_socketsMapMutex.lock();
  m_socketsMap[connectionId] = socket;
  m_socketsMapMutex.unlock();

  if (socket->connect())
  {
    return socket.get();
  }
  else
  {
    m_socketsMapMutex.lock();
    m_socketsMap.remove(connectionId);
    m_socketsMapMutex.unlock();
    return NULL;
  }
}

bool TransportLayer::remove(ISocket *socketPointer)
{
  ConnectionId connectionId(((Socket*) socketPointer)->destinationAddress(),
                            ((Socket*) socketPointer)->destinationPort(),
                            ((Socket*) socketPointer)->sourcePort());
  m_socketsMapMutex.lock();
  if (!m_socketsMap.contains(connectionId))
  {
    m_socketsMapMutex.unlock();
    return false;
  }
  SocketPtr socket = m_socketsMap[connectionId];
  m_socketsMapMutex.unlock();
  bool success = socket->disconnect();
  m_socketsMapMutex.lock();
  m_socketsMap.remove(connectionId);
  m_socketsMapMutex.unlock();
  return success;
}

Socket *TransportLayer::createSocket(Address address, Port sourcePort,
                                     Port destinationPort, uint sequence)
{
  SocketPtr socket(new Socket(m_network, address, destinationPort,
                              sourcePort, Socket::Type::Server));
  socket->setDestinationSequence(sequence);
  ConnectionId connectionId(address, destinationPort, sourcePort);
  QMutexLocker locker(&m_socketsMapMutex);
  m_socketsMap[connectionId] = socket;
  return socket.get();
}
