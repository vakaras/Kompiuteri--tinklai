#ifndef NEIGHBOURINFOPACKET_H
#define NEIGHBOURINFOPACKET_H

#include <types.h>
#include <networklayer/networklayer.h>
#include <interfaces/ILLCSublayer.h>
#include <tuple>

#define MAX_NEIGHBOURS 20


struct NeighbourInfoPacket
{

  struct NeighbourInfo
  {
    _M ILLCSublayer::Address      m_address;
    _M uint                       m_distance;

    _M Vacuum                     NeighbourInfo();
    _M Vacuum                     NeighbourInfo(
                                    const ILLCSublayer::Address &address,
                                    uint distance);
  } __attribute__((packed));

  _T NeighbourInfo*                           NeighbourInfoPtr;

  _M NetworkLayer::FrameType      m_type;
  _M INetworLayer::Address        m_senderAddress;
  _M uint                         m_sequenceNumber;
  _M MSec                         m_expires;
  _M ushort                       m_length;
  _M NeighbourInfoPtr             m_neighbours;

  _M Vacuum                   NeighbourInfoPacket();
  _M Vacuum                   NeighbourInfoPacket(
                                const NeighbourInfoPacket& packet);
  _M Vacuum                   NeighbourInfoPacket(
                                const INetworLayer::Address &senderAddress,
                                uint sequenceNumber);
  _M Vacuum                   ~NeighbourInfoPacket();
  _S NeighbourInfoPacket      fromBytes(BytePtr bytes);
  _M uint                     toBytes(BytePtr &bytes);
  _M void                     append(const ILLCSublayer::Address &address,
                                     uint distance);
  _S uint                     headerLength();
  _M uint                     length();
} __attribute__((packed));

#endif // NEIGHBOURINFOPACKET_H
