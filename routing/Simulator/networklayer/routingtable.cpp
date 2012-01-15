#include "routingtable.h"
#include <networklayer/neighbourinfopacket.h>
#include <networklayer/neighbourinfoackpacket.h>
#include <QDateTime>

RoutingTable::RoutingTable(INetworLayer::Address address, QObject *parent) :
    QObject(parent), m_address(address), m_process(this)
{
  QMutexLocker locker(&m_mutex);
  m_process.start();
}

RoutingTable::~RoutingTable()
{
  QMutexLocker locker(&m_mutex);
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
    qDebug() << "Get:" << packet.m_senderAddress;
    RouterInfo &info = m_routerInfoMap[packet.m_senderAddress];
    return info.m_sequenceNumber <= packet.m_sequenceNumber;
  }
  else
  {
    qDebug() << "Create:" << packet.m_senderAddress;
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
  qDebug() << "PRE:"
           << packet.m_senderAddress
           << m_routerInfoMap.contains(packet.m_senderAddress);
  if (!checkNeighbourInfoPacket(packet))
    return;
  bool duplicate = false;
  RouterInfo &info = m_routerInfoMap[packet.m_senderAddress];
  qDebug() << "IP:" << info.m_ipAddress << info.m_sequenceNumber;
  qDebug() << ":"
           << packet.m_senderAddress
           << m_routerInfoMap.contains(packet.m_senderAddress);
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

  qDebug() << "IP:" << info.m_ipAddress << info.m_sequenceNumber
           << duplicate;

  BytePtr bytes;
  uint len = ackPacket.toBytes(bytes);
  connection->send(senderAddress, bytes.get(), len);

  if (!duplicate)
    update();
}

void RoutingTable::checkACKPackage(ILLCSublayer::Address senderAddress,
                                   const NeighbourInfoACKPacket &packet)
{
  QMutexLocker locker(&m_mutex);
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
  removeOld();
  QMultiMap<uint, INetworLayer::Address> distances;
  QSet<INetworLayer::Address> analysed;

  for (auto routerInfo : m_routerInfoMap)
  {
    routerInfo.m_distance = UINT_MAX;
    routerInfo.m_through = 0;
  }
  for (auto neighbour : m_neighbourMap)
  {
    if (m_routerInfoMap.contains(neighbour.m_IPAddress))
    {
      m_routerInfoMap[neighbour.m_IPAddress].m_distance = (
            neighbour.m_distance);
      m_routerInfoMap[neighbour.m_IPAddress].m_through = (
            neighbour.m_IPAddress);
      distances.insert(neighbour.m_distance, neighbour.m_IPAddress);
    }
  }

  while (!distances.isEmpty())
  {
    auto it = distances.lowerBound(0);
    uint distance = it.key();
    INetworLayer::Address address = it.value();
    RouterInfo &routerInfo = m_routerInfoMap[address];
    if (!analysed.contains(address))
    {
      for (auto neighbourIt = routerInfo.m_distanceMap.begin();
           neighbourIt != routerInfo.m_distanceMap.end();
           neighbourIt++)
      {
        if (m_routerInfoMap.contains(neighbourIt.key()))
        {
          RouterInfo &neighbour = m_routerInfoMap[neighbourIt.key()];
          if (m_neighbourMap.contains(neighbour.m_ipAddress) &&
              m_neighbourMap.contains(routerInfo.m_ipAddress) &&
              m_neighbourMap[routerInfo.m_ipAddress].m_connection ==
              m_neighbourMap[neighbour.m_ipAddress].m_connection)
          {
            // If “routerInfo” and “neighbour” are my neighbours and they
            // are connected to the same medium.
          }
          else
          {
            if (neighbour.m_distance > distance + neighbourIt.value())
            {
              neighbour.m_distance = distance + neighbourIt.value();
              neighbour.m_through = routerInfo.m_through;
              distances.insert(neighbour.m_distance, neighbourIt.key());
            }
          }
        }
      }
      analysed << address;
    }
    distances.erase(it);
  }
}

void RoutingTable::removeOld()
{
  MSec now = QDateTime::currentMSecsSinceEpoch();
  QList<INetworLayer::Address> removeList;
  for (auto it = m_routerInfoMap.begin(); it != m_routerInfoMap.end(); it++)
  {
    if (it.value().m_expires <= now)
    {
      removeList.append(it.key());
    }
  }
  for (auto key : removeList)
  {
    m_routerInfoMap.remove(key);
  }
}

uint RoutingTable::forwardPackages()
{
  QMutexLocker locker(&m_mutex);
  bool sent = false;
  for (auto routerInfo : m_routerInfoMap)
  {
    for (auto neighbourInfo : m_neighbourMap)
    {
      if (routerInfo.m_ipAddress == neighbourInfo.m_IPAddress)
      {
        continue;
      }
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
