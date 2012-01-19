#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <tuple>
#include <interfaces/ITransportLayer.h>
#include <transportlayer/tcppacket.h>


class TransportLayer;

class Listener : public QObject, public IListener
{

  Q_OBJECT

private:

  _T std::tuple<
      ITransportLayer::Address,
      ITransportLayer::Port,
      uint>                           ConnectionInfo;
  _T QList<ConnectionInfo>            ConnectionInfoList;


  _M TransportLayer*        m_layer;
  _M ITransportLayer::Port  m_port;

  _M QMutex                 m_incommingConnectionsMutex;
  _M QWaitCondition         m_incommingConnectionsWaitCondition;
  _M ConnectionInfoList     m_incommingConnections;


public:
  explicit Listener(TransportLayer *layer,
                    ITransportLayer::Port port, QObject *parent = 0);
  _M ISocket* get();
  _M void     addConnectionInfo(ITransportLayer::Address address,
                                ITransportLayer::Port port,
                                uint sequence);
};

typedef std::shared_ptr<Listener>   ListenerPtr;

#endif // LISTENER_H
