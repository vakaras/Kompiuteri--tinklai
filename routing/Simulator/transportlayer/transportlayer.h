#ifndef TRANSPORTLAYER_H
#define TRANSPORTLAYER_H

#include <tuple>
#include <QObject>
#include <QMutex>
#include <QList>
#include <QSet>
#include <QMap>
#include <interfaces/INetworkLayer.h>
#include <interfaces/ITransportLayer.h>
#include <transportlayer/socket.h>
#include <transportlayer/listener.h>
#include <transportlayer/transportreader.h>


class TransportLayer : public QObject, public ITransportLayer
{

  Q_OBJECT

public:

  // Destination address, destination port, source port.
  _T std::tuple<Address, Port, Port>  ConnectionId;
  _T QMap<ConnectionId, SocketPtr>    SocketMap;
  _T QMap<Port, ListenerPtr>          ListenerMap;
  _T QSet<Port>                       PortSet;

private:

  _M INetworkLayer*   m_network;

  _M TransportReader  m_reader;

  _M QMutex           m_listenersMapMutex;
  _M ListenerMap      m_listenersMap;

  _M QMutex           m_socketsMapMutex;
  _M SocketMap        m_socketsMap;

  // Port management.
  _M QMutex           m_portSetMutex;
  _M PortSet          m_portSet;
  _M uint             m_lastPort;
  _M Port             getFreePort();
  _M void             freePort(Port port);

public:
  explicit TransportLayer(INetworkLayer *network, QObject *parent = 0);
  _M Vacuum       ~TransportLayer();

  _M void         parseSegment(Address address, BytePtr bytes, uint len);
  _M Socket*      createSocket(Address address, Port sourcePort,
                               Port destinationPort, uint sequence);

  _M IListener*   createListener(Port port);
  _M ISocket*     connect(Address address, Port port);
  _M bool         remove(ISocket* socket);
};

#endif // TRANSPORTLAYER_H
