#include "neighbourinfopacket.h"
#include <networklayer/networklayer.h>
#include <QDateTime>

NeighbourInfoPacket::NeighbourInfo::NeighbourInfo():
  m_address(0), m_distance(UINT_MAX)
{
}

NeighbourInfoPacket::NeighbourInfo::NeighbourInfo(
  const INetworLayer::Address &address, uint distance):
  m_address(address), m_distance(distance)
{
}

NeighbourInfoPacket::NeighbourInfoPacket():
  m_type(NetworkLayer::FrameType::Unset), m_senderAddress(0),
  m_sequenceNumber(0), m_expires(0), m_length(0)
{
  m_neighbours = new NeighbourInfo[MAX_NEIGHBOURS];
}

NeighbourInfoPacket &NeighbourInfoPacket::operator =(
  const NeighbourInfoPacket &packet)
{
  memcpy((void *) this, (void *) &packet, headerLength());
  delete [] m_neighbours;
  m_neighbours = new NeighbourInfo[MAX_NEIGHBOURS];
  memcpy((void *) m_neighbours, (void *) packet.m_neighbours,
         MAX_NEIGHBOURS*sizeof(NeighbourInfo));
  return *this;
}

NeighbourInfoPacket::NeighbourInfoPacket(const NeighbourInfoPacket &packet)
{
  memcpy((void *) this, (void *) &packet, headerLength());
  m_neighbours = new NeighbourInfo[MAX_NEIGHBOURS];
  memcpy((void *) m_neighbours, (void *) packet.m_neighbours,
         MAX_NEIGHBOURS*sizeof(NeighbourInfo));
}

NeighbourInfoPacket::NeighbourInfoPacket(
  const INetworLayer::Address &senderAddress, uint sequenceNumber):
  m_type(NetworkLayer::FrameType::NeighbourInfo),
  m_senderAddress(senderAddress), m_sequenceNumber(sequenceNumber),
  m_expires(QDateTime::currentMSecsSinceEpoch() + ROUTES_UPDATE_PERIOD * 6),
  m_length(0)
{
  m_neighbours = new NeighbourInfo[MAX_NEIGHBOURS];
}

NeighbourInfoPacket::~NeighbourInfoPacket()
{
  delete [] m_neighbours;
}

NeighbourInfoPacket NeighbourInfoPacket::fromBytes(BytePtr bytes)
{
  NeighbourInfoPacket *pointer = (NeighbourInfoPacket*) bytes.get();
  NeighbourInfoPacket packet;
  memcpy((void *) &packet, (void *) pointer, headerLength());
  Q_ASSERT(packet.m_length <= MAX_NEIGHBOURS);
  NeighbourInfo *info = (NeighbourInfo *)(bytes.get() + headerLength());
//memcpy((void *) packet.m_neighbours,
//       (void *) (bytes.get() + headerLength()),
//       packet.m_length);
  for (uint i = 0; i < packet.m_length; i++)
  {
    packet.m_neighbours[i] = info[i];
    qDebug() << i << packet.m_neighbours[i].m_address
             << packet.m_neighbours[i].m_distance;
  }
  return packet;
}

uint NeighbourInfoPacket::toBytes(BytePtr &bytes)
{
  Byte *pointer = new Byte[length()];
  bytes = BytePtr(pointer, sharedArrayDeleter<Byte>);
  memcpy((void *) pointer, (void *) this, headerLength());

  NeighbourInfo *info = (NeighbourInfo *)(pointer + headerLength());
//memcpy((void *) pointer, (void *) m_neighbours,
//       sizeof(NeighbourInfo) * m_length);
  for (uint i = 0; i < m_length; i++)
  {
    info[i] = m_neighbours[i];
  }
  return length();
}

void NeighbourInfoPacket::append(
  const INetworLayer::Address &address, uint distance)
{
  if (m_length + 1 < MAX_NEIGHBOURS)
  {
    m_neighbours[m_length++] = NeighbourInfo(address, distance);
  }
}

uint NeighbourInfoPacket::headerLength()
{
  return sizeof(NeighbourInfoPacket) - sizeof(m_neighbours);
}

uint NeighbourInfoPacket::length()
{
  return headerLength() + sizeof(NeighbourInfo) * m_length;
}
