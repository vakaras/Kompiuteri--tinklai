#include "neighbourinfoackpacket.h"

NeighbourInfoACKPacket::NeighbourInfoACKPacket():
  m_type(NetworkLayer::FrameType::NeighbourInfoACK), m_sequenceNumber(0)
{
}

NeighbourInfoACKPacket::NeighbourInfoACKPacket(
  INetworLayer::Address senderAddress, const NeighbourInfoPacket &packet):
  m_type(NetworkLayer::FrameType::NeighbourInfoACK),
  m_dataAuthorAddress(packet.m_senderAddress),
  m_senderAddress(senderAddress), m_sequenceNumber(packet.m_sequenceNumber)
{
}

NeighbourInfoACKPacket NeighbourInfoACKPacket::fromBytes(BytePtr bytes)
{
  return *((NeighbourInfoACKPacket *) bytes.get());
}

uint NeighbourInfoACKPacket::toBytes(BytePtr &bytes)
{
  Byte *pointer = new Byte[length()];
  bytes = BytePtr(pointer, sharedArrayDeleter<Byte>);
  memcpy((void *) pointer, (void *) this, length());
  return length();
}

uint NeighbourInfoACKPacket::length()
{
  return sizeof(NeighbourInfoACKPacket);
}
