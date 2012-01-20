#ifndef NETWORKLAYER_H
#define NETWORKLAYER_H

#include <tuple>
#include <QObject>
#include <QMap>
#include <QList>
#include <QMutex>
#include <QWaitCondition>
#include <interfaces/ILLCSublayer.h>
#include <interfaces/INetworkLayer.h>
#include <networklayer/types.h>
#include <networklayer/neighbourinfo.h>
#include <networklayer/routingprocess.h>
#include <networklayer/connectionwrapper.h>
#include <networklayer/routingtable.h>

extern uint ROUTES_UPDATE_PERIOD;
extern int MAX_READ_BUFFER_SIZE;


class TestNetworkLayer;


class NetworkLayer : public QObject, public INetworkLayer
{

  Q_OBJECT

public:

  _T NetworkLayerFrameType  FrameType;

private:

  _T QMap<Address, NeighbourInfo>         NeighbourMap;
  _T QList<Address>                       AddressList;
  _T QList<ConnectionWrapper>             ConnectionList;
  _T std::tuple<Address, BytePtr, uint>   DataFrame;
  _T QList<DataFrame>                     DataFrameList;

  _M INetworkLayer::Address m_address;
  _M ConnectionList         m_connectionList;
  _M QMutex                 m_connectionListMutex;

  _M NeighbourMap           m_neighbourMap;
  _M QMutex                 m_neighbourMapMutex;
  _M void                   addNeighbour(NeighbourInfo neighbour);
  _M void                   removeNeighbour(INetworkLayer::Address address);

  /**
    Moment, when router should recalculate distances to its neighbours.
    */
  _M MSec                   m_calculationExpires;
  _M QMutex                 m_calculationExpiresMutex;

  /*
    Neighbours info sequence number.
    */
  _M uint                   m_sequenceNumber;

  _M RoutingTable           m_routingTable;
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
  _M void                   parseNeighbourListACK(
                              const IMACSublayer::Address &address,
                              BytePtr bytes, uint len);
  _M void                   parseIP(BytePtr bytes, uint len);

  /**
    For sending and receiving data packages.
    */
  _M QMutex                 m_sendMutex;
  _M DataFrameList          m_readBuffer;
  _M QMutex                 m_readBufferMutex;
  _M QWaitCondition         m_readBufferWaitCondition;

  _F class                  TestNetworkLayer;

public:

  explicit NetworkLayer(INetworkLayer::Address address, QObject *parent = 0);
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
