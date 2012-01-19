#include "listener.h"
#include <transportlayer/transportlayer.h>

Listener::Listener(TransportLayer *layer, ITransportLayer::Port port,
                   QObject *parent) :
  QObject(parent), m_layer(layer), m_port(port)
{
}

ISocket* Listener::get()
{
  QMutexLocker locker(&m_incommingConnectionsMutex);
  while (m_incommingConnections.isEmpty())
  {
    m_incommingConnectionsWaitCondition.wait(&m_incommingConnectionsMutex);
  }
  ITransportLayer::Address address;
  ITransportLayer::Port port;
  uint sequence;
  std::tie(address, port, sequence) = m_incommingConnections.first();
  m_incommingConnections.removeFirst();
  Socket *socket = m_layer->createSocket(address, m_port, port, sequence);
  socket->sendConnectResponse();
  return socket;
}

void Listener::addConnectionInfo(ITransportLayer::Address address,
                                 ITransportLayer::Port port, uint sequence)
{
  qDebug() << "Adding connection info.";
  QMutexLocker locker(&m_incommingConnectionsMutex);
  m_incommingConnections.append(ConnectionInfo(address, port, sequence));
  m_incommingConnectionsWaitCondition.wakeOne();
}
