#include "helloneighbourpacket.h"
#include <QDateTime>

HelloNeighbourPacket::HelloNeighbourPacket():
  m_type(NetworkLayer::FrameType::Unset), m_MACAddress(0), m_IPAddress(0),
  m_sent(0), m_answerSent(0)
{
}

HelloNeighbourPacket::HelloNeighbourPacket(
  NetworkLayer::FrameType type, IMACSublayer::Address MACAddress,
  INetworLayer::Address IPAddress):
  m_type(type), m_MACAddress(MACAddress), m_IPAddress(IPAddress),
  m_sent(0), m_answerSent(0)
{
}

HelloNeighbourPacket HelloNeighbourPacket::createRequest(
  IMACSublayer::Address MACAddress, INetworLayer::Address IPAddress)
{
  HelloNeighbourPacket request(NetworkLayer::FrameType::HelloRequest,
                               MACAddress, IPAddress);
  request.m_sent = QDateTime::currentMSecsSinceEpoch();
  return request;
}

HelloNeighbourPacket HelloNeighbourPacket::createAnswer(
  const HelloNeighbourPacket &request,
  IMACSublayer::Address MACAddress,
  INetworLayer::Address IPAddress)
{
  HelloNeighbourPacket answer = request;
  answer.m_type = NetworkLayer::FrameType::HelloAnswer;
  answer.m_IPAddress = IPAddress;
  answer.m_MACAddress = MACAddress;
  answer.m_answerSent = QDateTime::currentMSecsSinceEpoch();
  return answer;
}

HelloNeighbourPacket HelloNeighbourPacket::fromBytes(BytePtr bytes)
{
  HelloNeighbourPacket packet;
  packet = *((HelloNeighbourPacket *) bytes.get());
  return packet;
}

Byte* HelloNeighbourPacket::asBytes()
{
  return (Byte *) this;
}

uint HelloNeighbourPacket::len()
{
  return sizeof(HelloNeighbourPacket);
}
