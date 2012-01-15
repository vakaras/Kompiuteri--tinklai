#ifndef NEIGHBOURINFO_H
#define NEIGHBOURINFO_H

#include <interfaces/INetworkLayer.h>
#include <interfaces/ILLCSublayer.h>

#define NEIGHBOUR_INFO_LIFETIME 20000


struct NeighbourInfo
{

  _M IMACSublayer::Address    m_MACAddress;
  _M ILLCSublayerPtr          m_connection;
  _M INetworLayer::Address    m_IPAddress;
  _M uint                     m_distance;

    /// Time moment, after which this info is considered to be useless.
  _M MSec                     m_expires;

  _M NeighbourInfo();
  _M NeighbourInfo(IMACSublayer::Address MACAddress,
                   ILLCSublayerPtr connection,
                   INetworLayer::Address IPAdress,
                   uint distance);
};

#endif // NEIGHBOURINFO_H
