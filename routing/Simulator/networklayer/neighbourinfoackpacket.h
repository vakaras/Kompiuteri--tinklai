#ifndef NEIGHBOURINFOACKPACKET_H
#define NEIGHBOURINFOACKPACKET_H


#include <networklayer/networklayer.h>
#include <networklayer/neighbourinfopacket.h>


struct NeighbourInfoACKPacket
{
  _M NetworkLayer::FrameType  m_type;
  _M INetworkLayer::Address    m_dataAuthorAddress;
  _M INetworkLayer::Address    m_senderAddress;
  _M uint                     m_sequenceNumber;

  _M Vacuum                   NeighbourInfoACKPacket();
  _M Vacuum                   NeighbourInfoACKPacket(
                                INetworkLayer::Address senderAddress,
                                const NeighbourInfoPacket &packet);
  _S NeighbourInfoACKPacket   fromBytes(BytePtr bytes);
  _M uint                     toBytes(BytePtr &bytes);
  _S uint                     length();

} __attribute__((packed));

#endif // NEIGHBOURINFOACKPACKET_H
