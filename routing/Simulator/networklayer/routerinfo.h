#ifndef ROUTERINFO_H
#define ROUTERINFO_H

#include <QObject>
#include <QSet>
#include <QMap>
#include <interfaces/INetworkLayer.h>
#include <interfaces/ILLCSublayer.h>
#include <networklayer/neighbourinfopacket.h>

struct RouterInfo
{

  _T QSet<ILLCSublayer::Address>        AddressSet;
  _T QMap<INetworLayer::Address, uint>  NeighbourDistanceMap;

  _M INetworLayer::Address        m_ipAddress;
  _M uint                         m_sequenceNumber;
  _M MSec                         m_expires;

  /** Which neighbours have this information. */
  _M AddressSet                   m_knowsInfoSet;

  _M NeighbourDistanceMap         m_distanceMap;

  // UINT_MAX – means that this node ist not yet analyzed.
  _M uint                         m_distance;
  /**
    Address of the neighbour through which packages should be forwarded
    to this router.
    */
  _M INetworLayer::Address        m_through;

  _M NeighbourInfoPacket          m_packet;

  _M Vacuum                       RouterInfo();
  _M Vacuum                       RouterInfo(
                                    INetworLayer::Address ipAddress);

};

#endif // ROUTERINFO_H
