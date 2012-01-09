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
#include <networklayer/connectionwrapper.h>

#define ROUTES_UPDATE_PERIOD 10000


class NetworkLayer : public QObject, INetworLayer
{

  Q_OBJECT

public:

  _E class FrameType : Byte
  {
    Unset,
    HelloRequest,
    HelloAnswer,
    NeighbourInfo
  };

private:

  _T QMap<INetworLayer::Address, NeighbourInfo>     NeighbourMap;
  _T QList<INetworLayer::Address>                   AddressList;
  _T QList<ConnectionWrapper>                       ConnectionList;

  _M INetworLayer::Address  m_address;
  _M ConnectionList         m_connectionList;
  _M QMutex                 m_connectionListMutex;

  _M NeighbourMap           m_neighbourMap;
  _M QMutex                 m_neighbourMapMutex;

  /**
    Moment, when router should recalculate distances to its neighbours.
    */
  _M MSec                   m_calculationExpires;
  _M QMutex                 m_calculationExpiresMutex;

  /*
    Neighbours info sequence number.
    */
  _M uint                   m_sequenceNumber;

  _F class                  RoutingProcess;
  _M RoutingProcess         m_routingProcess;
  _M QMutex                 m_routingProcessMutex;
  _M QWaitCondition         m_routingProcessWaitCondition;

  _M MSec                   calculationExpires();
  _M void                   setCalculationExpires(
                              MSec moment);

  _M void                   removeOldNeighbours();
  _M void                   helloNeighbours();
  _M void                   sendNeighboursList();

  _F class                  NetworkReader;

  _M void                   parseFrame(
                              ILLCSublayerPtr connection,
                              const IMACSublayer::Address &address,
                              BytePtr bytes, uint len);
  _M void                   parseHelloRequest(
                              ILLCSublayerPtr connection,
                              const IMACSublayer::Address &address,
                              BytePtr bytes, uint len);
  _M void                   parseHelloAnswer(
                              ILLCSublayerPtr connection,
                              const IMACSublayer::Address &address,
                              BytePtr bytes, uint len);
  _M void                   parseNeighbourList(
                              ILLCSublayerPtr connection,
                              const IMACSublayer::Address &address,
                              BytePtr bytes, uint len);

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

typedef std::shared_ptr<NetworkLayer>   NetworkLayerPtr;

#endif // NETWORKLAYER_H
