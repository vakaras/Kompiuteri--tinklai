#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <tuple>
#include <QObject>
#include <QMutex>
#include <QMap>
#include <interfaces/ILLCSublayer.h>
#include <interfaces/INetworkLayer.h>
#include <networklayer/neighbourinfo.h>
#include <networklayer/routerinfo.h>
#include <networklayer/routingtableprocess.h>
#include <networklayer/ippacket.h>


class NeighbourInfoPacket;
class NeighbourInfoACKPacket;
class TestNetworkLayer;


class RoutingTable : public QObject
{

  Q_OBJECT

private:

  _T std::tuple<uint, INetworkLayer::Address>        AddressDistance;
  _T QMap<INetworkLayer::Address, NeighbourInfo>     NeighbourMap;
  _T QMap<INetworkLayer::Address, RouterInfo>        RouterInfoMap;

  _M INetworkLayer::Address m_address;
  _M QMutex                 m_mutex;

  _M NeighbourMap           m_neighbourMap;
  _M RouterInfoMap          m_routerInfoMap;

  _M RoutingTableProcess    m_process;

  /**
    Checks if info have to updated from this packet. If entry does not
    exists – creates.
    */
  _M bool                   checkNeighbourInfoPacket(
                              const NeighbourInfoPacket &packet);

  _F class                  TestNetworkLayer;

public:

  explicit RoutingTable(INetworkLayer::Address address, QObject *parent = 0);
  _M Vacuum   ~RoutingTable();

  /**
    Forward data package. Returns true if successful.
  */
  _M bool     forward(IPPacket packet);

  /**
    Creates entry for neighbour if it doesn't exist.
    @returns false if already existed.
    */
  _M bool     add(NeighbourInfo &info);

  /**
    Removes entry for neighbour if it exists.
    @returns true if existed.
    */
  _M bool     remove(NeighbourInfo &info);

  /** Automatically calls update(); */
  _M void     updateRouterInfo(ILLCSublayerPtr connection,
                               ILLCSublayer::Address senderAddress,
                               const NeighbourInfoPacket &packet);
  _M void     checkACKPackage(ILLCSublayer::Address senderAddress,
                              const NeighbourInfoACKPacket &packet);

  /** Removes old entries and runs Dijkstra. */
  _M void     update();

  _M void     removeOld();

  /** Forwards packages with neighbours info.
    @returns thread sleep time in miliseconds.
   */
  _M uint     forwardPackages();

};

#endif // ROUTINGTABLE_H
