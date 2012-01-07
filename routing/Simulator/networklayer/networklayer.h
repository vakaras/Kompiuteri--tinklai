#ifndef NETWORKLAYER_H
#define NETWORKLAYER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QMutex>
#include <QWaitCondition>
#include <interfaces/ILLCSublayer.h>
#include <interfaces/INetworkLayer.h>
#include <networklayer/neighbourinfo.h>
#include <networklayer/routingprocess.h>

#define ROUTES_UPDATE_PERIOD 10000


class NetworkLayer : public QObject, INetworLayer
{

  Q_OBJECT

private:

  _T QMap<INetworLayer::Address, NeighbourInfo>     NeighbourMap;
  _T QList<ILLCSublayerPtr>                         ConnectionList;

  _M INetworLayer::Address                    m_address;
  _M ConnectionList                           m_connectionList;
  _M QMutex                                   m_connectionListMutex;

  _M NeighbourMap                             m_neighbourMap;
  _M QMutex                                   m_neighbourMapMutex;

  /**
    Moment, when router should recalculate distances to its neighbours.
    */
  _M MSec                                     m_calculationExpires;
  _M QMutex                                   m_calculationExpiresMutex;

  _F class                                    RoutingProcess;
  _M RoutingProcess                           m_routingProcess;
  _M QMutex                                   m_routingProcessMutex;
  _M QWaitCondition                           m_routingProcessWaitCondition;

  _M MSec                                     calculationExpires();
  _M void                                     setCalculationExpires(
                                                MSec moment);

public:

  explicit NetworkLayer(INetworLayer::Address address, QObject *parent = 0);
  _M Vacuum ~NetworkLayer();

  _M void   append(ILLCSublayerPtr connection);
  _M bool   remove(ILLCSublayerPtr connection);

  _M bool   send(Address address, Byte *bytes, uint len);
  _M uint   receive(Address &address, BytePtr &bytes,
                    ulong time=ULONG_MAX);
  _M void   restart();

};

#endif // NETWORKLAYER_H
