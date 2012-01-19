#ifndef NEIGHBOURINFOPACKET_H
#define NEIGHBOURINFOPACKET_H

#include <interfaces/INetworkLayer.h>
#include <networklayer/types.h>

#define MAX_NEIGHBOURS 20


struct NeighbourInfoPacket
{

  struct NeighbourInfo
  {
    _M INetworkLayer::Address      m_address;
    _M uint                       m_distance;

    _M Vacuum                     NeighbourInfo();
    _M Vacuum                     NeighbourInfo(
                                    const INetworkLayer::Address &address,
                                    uint distance);
  } __attribute__((packed));

  _T NeighbourInfo*               NeighbourInfoPtr;

  _M NetworkLayerFrameType        m_type;
  _M INetworkLayer::Address        m_senderAddress;
  _M uint                         m_sequenceNumber;
  _M MSec                         m_expires;
  _M ushort                       m_length;
  _M NeighbourInfoPtr             m_neighbours;

  _M Vacuum                   NeighbourInfoPacket();
  _M Vacuum                   NeighbourInfoPacket(
                                const NeighbourInfoPacket& packet);
  _M Vacuum                   NeighbourInfoPacket(
                                const INetworkLayer::Address &senderAddress,
                                uint sequenceNumber);
  _M Vacuum                   ~NeighbourInfoPacket();
  _M NeighbourInfoPacket&     operator=(const NeighbourInfoPacket &packet);
  _S NeighbourInfoPacket      fromBytes(BytePtr bytes);
  _M uint                     toBytes(BytePtr &bytes);
  _M void                     append(const INetworkLayer::Address &address,
                                     uint distance);
  _S uint                     headerLength();
  _M uint                     length();
} __attribute__((packed));

#endif // NEIGHBOURINFOPACKET_H
