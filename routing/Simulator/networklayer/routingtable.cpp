#include "routingtable.h"
#include <networklayer/neighbourinfopacket.h>
#include <networklayer/neighbourinfoackpacket.h>

RoutingTable::RoutingTable(INetworLayer::Address address, QObject *parent) :
    QObject(parent), m_address(address), m_process(this)
{
  m_process.start();
}

RoutingTable::~RoutingTable()
{
  m_process.stop();
}

bool RoutingTable::add(NeighbourInfo &info)
{
  QMutexLocker locker(&m_mutex);

  if (m_neighbourMap.contains(info.m_IPAddress))
  {
    return false;
  }
  else
  {
    m_neighbourMap[info.m_IPAddress] = info;
    return true;
  }
}

bool RoutingTable::remove(NeighbourInfo &info)
{
  QMutexLocker locker(&m_mutex);

  if (m_neighbourMap.contains(info.m_IPAddress))
  {
    m_neighbourMap.remove(info.m_IPAddress);
    return true;
  }
  else
  {
    return false;
  }
}

bool RoutingTable::checkNeighbourInfoPacket(
  const NeighbourInfoPacket &packet)
{
  if (m_routerInfoMap.contains(packet.m_senderAddress))
  {
    RouterInfo &info = m_routerInfoMap[packet.m_senderAddress];
    return info.m_sequenceNumber <= packet.m_sequenceNumber;
  }
  else
  {
    m_routerInfoMap[packet.m_senderAddress] = RouterInfo(
          packet.m_senderAddress);
    return true;
  }
}

void RoutingTable::updateRouterInfo(ILLCSublayerPtr connection,
                                    ILLCSublayer::Address senderAddress,
                                    const NeighbourInfoPacket &packet)
{
  QMutexLocker locker(&m_mutex);
  if (!checkNeighbourInfoPacket(packet))
    return;
  bool duplicate = false;
  RouterInfo &info = m_routerInfoMap[packet.m_senderAddress];
  if (info.m_sequenceNumber < packet.m_sequenceNumber)
  {
    info.m_sequenceNumber = packet.m_sequenceNumber;
    info.m_expires = packet.m_expires;
    info.m_knowsInfoSet.clear();
    info.m_distanceMap.clear();
    for (uint i = 0; i < packet.m_length; i++)
    {
      info.m_distanceMap[packet.m_neighbours[i].m_address] = (
            packet.m_neighbours[i].m_distance);
    }
    info.m_distance = UINT_MAX;
    info.m_through = 0;
    info.m_packet = packet;
  }
  else
  {
    duplicate = true;
  }
  info.m_knowsInfoSet << senderAddress;
  NeighbourInfoACKPacket ackPacket = NeighbourInfoACKPacket(
        m_address, packet);

  BytePtr bytes;
  uint len = ackPacket.toBytes(bytes);
  connection->send(senderAddress, bytes.get(), len);

  if (!duplicate)
    update();
}

void RoutingTable::checkACKPackage(ILLCSublayer::Address senderAddress,
                                   const NeighbourInfoACKPacket &packet)
{
  if (!m_routerInfoMap.contains(packet.m_dataAuthorAddress))
  {
    return;
  }
  RouterInfo &routerInfo = m_routerInfoMap[packet.m_dataAuthorAddress];
  if (routerInfo.m_sequenceNumber == packet.m_sequenceNumber)
  {
    routerInfo.m_knowsInfoSet << senderAddress;
  }
}

void RoutingTable::update()
{
  //QMultiMap, std::tie(a, b) = AddressDistance;
  // TODO
}

uint RoutingTable::forwardPackages()
{
  bool sent = false;
  for (auto routerInfo : m_routerInfoMap)
  {
    for (auto neighbourInfo : m_neighbourMap)
    {
      if (!routerInfo.m_knowsInfoSet.contains(neighbourInfo.m_MACAddress))
      {
        BytePtr bytes;
        uint len = routerInfo.m_packet.toBytes(bytes);
        neighbourInfo.m_connection->send(neighbourInfo.m_MACAddress,
                                         bytes.get(), len);
        sent = true;
      }
    }
  }
  return sent ? 300 : 1000;
}
