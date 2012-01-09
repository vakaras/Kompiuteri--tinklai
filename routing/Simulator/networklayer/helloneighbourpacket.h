#ifndef HELLONEIGHBOURPACKET_H
#define HELLONEIGHBOURPACKET_H

#include <types.h>
#include <interfaces/ILLCSublayer.h>
#include <interfaces/INetworkLayer.h>
#include <networklayer/networklayer.h>

struct HelloNeighbourPacket
{

  _M NetworkLayer::FrameType  m_type;
  _M IMACSublayer::Address    m_MACAddress;
  _M INetworLayer::Address    m_IPAddress;
  _M MSec                     m_sent;
  _M MSec                     m_answerSent;

  _M Vacuum                   HelloNeighbourPacket();
  _M Vacuum                   HelloNeighbourPacket(
                                NetworkLayer::FrameType type,
                                IMACSublayer::Address MACAddress,
                                INetworLayer::Address IPAddress);
  _S HelloNeighbourPacket     createRequest(
                                IMACSublayer::Address MACAddress,
                                INetworLayer::Address IPAddress);
  _S HelloNeighbourPacket     createAnswer(
                                const HelloNeighbourPacket &request,
                                IMACSublayer::Address MACAddress,
                                INetworLayer::Address IPAddress);
  _S HelloNeighbourPacket     fromBytes(BytePtr bytes);
  _M Byte*                    asBytes();
  _S uint                     len();
} __attribute__((packed));

#endif // HELLONEIGHBOURPACKET_H
