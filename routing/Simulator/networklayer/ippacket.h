#ifndef IPPACKET_H
#define IPPACKET_H

#include <networklayer/types.h>
#include <interfaces/INetworkLayer.h>

extern uint MAX_HOPS;

struct IPPacket
{

  _M NetworkLayerFrameType    m_type;
  _M uint                     m_dataLength;
  _M uint                     m_hops;   // How many hops left.
  _M INetworkLayer::Address   m_source;
  _M INetworkLayer::Address   m_destination;
  _M Byte*                    m_data;   // Pointer to EXTERNAL data.

  _M Vacuum                   IPPacket();
  _M Vacuum                   IPPacket(INetworkLayer::Address source,
                                       INetworkLayer::Address destination,
                                       Byte* data,
                                       uint dataLength);
  _S IPPacket                 fromBytes(BytePtr bytes);
  _M uint                     toBytes(BytePtr &bytes);
  _S uint                     headerLength();
  _M uint                     length();
} __attribute__((packed));

#endif // IPPACKET_H
