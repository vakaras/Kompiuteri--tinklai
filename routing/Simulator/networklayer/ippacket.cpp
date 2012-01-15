#include "ippacket.h"

IPPacket::IPPacket():
  m_type(NetworkLayerFrameType::IP), m_dataLength(0), m_hops(0),
  m_source(0), m_destination(0), m_data(NULL)
{
}

IPPacket::IPPacket(INetworLayer::Address source,
                   INetworLayer::Address destination,
                   Byte *data, uint dataLength):
  m_type(NetworkLayerFrameType::IP), m_dataLength(dataLength),
  m_hops(MAX_HOPS), m_source(source), m_destination(destination),
  m_data(data)
{
}

IPPacket IPPacket::fromBytes(BytePtr bytes)
{
  IPPacket *pointer = (IPPacket*) bytes.get();
  IPPacket packet;
  memcpy((void *) &packet, (void *) pointer, headerLength());
  packet.m_data = bytes.get() + headerLength();
  return packet;
}

uint IPPacket::toBytes(BytePtr &bytes)
{
  Byte *pointer = new Byte[length()];
  bytes = BytePtr(pointer, sharedArrayDeleter<Byte>);
  memcpy((void *) pointer, (void *) this, headerLength());
  memcpy((void *) (pointer + headerLength()), m_data, m_dataLength);
  return length();
}

uint IPPacket::headerLength()
{
  return sizeof(IPPacket) - sizeof(m_data);
}

uint IPPacket::length()
{
  return headerLength() + m_dataLength;
}
