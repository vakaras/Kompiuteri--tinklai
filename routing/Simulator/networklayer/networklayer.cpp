#include "networklayer/networklayer.h"
#include <networklayer/helloneighbourpacket.h>
#include <networklayer/neighbourinfopacket.h>
#include <networklayer/neighbourinfoackpacket.h>
#include <QDateTime>

NetworkLayer::NetworkLayer(INetworLayer::Address address, QObject *parent) :
    QObject(parent), m_address(address), m_calculationExpires(0),
    m_sequenceNumber(1), m_routingTable(address), m_routingProcess(this)
{
  m_routingProcess.start();
}

NetworkLayer::~NetworkLayer()
{
  m_routingProcess.stop();
  QMutexLocker locker(&m_connectionListMutex);
  m_connectionList.clear();
}

void NetworkLayer::append(ILLCSublayerPtr connection)
{
  QMutexLocker locker(&m_connectionListMutex);
  QMutexLocker locker2(&m_calculationExpiresMutex);

  m_connectionList.append(ConnectionWrapper(this, connection));
  m_calculationExpires = 0;
}

void NetworkLayer::addNeighbour(NeighbourInfo neighbour)
{
  m_neighbourMap[neighbour.m_MACAddress] = neighbour;
  m_routingTable.add(neighbour);
}

void NetworkLayer::removeNeighbour(INetworLayer::Address address)
{
  m_routingTable.remove(m_neighbourMap[address]);
  m_neighbourMap.remove(address);
}

bool NetworkLayer::remove(ILLCSublayerPtr connection)
{
  QMutexLocker locker1(&m_neighbourMapMutex);
  QMutexLocker locker2(&m_connectionListMutex);
  QMutexLocker locker3(&m_calculationExpiresMutex);

  AddressList list;
  for (auto it = m_neighbourMap.begin();
       it != m_neighbourMap.end(); it++)
  {
    if (it.value().m_connection == connection)
    {
      list.append(it.key());
    }
  }
  for (auto it : list)
  {
    removeNeighbour(it);
  }

  for (int i = 0; i < m_connectionList.size(); i++)
  {
    if (m_connectionList.at(i).connection() == connection)
    {
      m_connectionList.removeAt(i);
      m_calculationExpires = 0;
      m_routingProcessWaitCondition.wakeOne();
      return true;
    }
  }
  return false;
}

MSec NetworkLayer::calculationExpires()
{
  QMutexLocker locker(&m_calculationExpiresMutex);
  return m_calculationExpires;
}

void NetworkLayer::setCalculationExpires(MSec moment)
{
  QMutexLocker locker(&m_calculationExpiresMutex);
  m_calculationExpires = moment;
}

bool NetworkLayer::send(Address address, Byte *bytes, uint len)
{
  // TODO
  return false;
}

uint NetworkLayer::receive(Address &address, BytePtr &bytes, ulong time)
{
  // TODO
  return 0;
}

void NetworkLayer::restart()
{
  // TODO
}

void NetworkLayer::removeOldNeighbours()
{
  QMutexLocker locker(&m_neighbourMapMutex);
  AddressList list;
  MSec now = QDateTime::currentMSecsSinceEpoch();
  for (auto it = m_neighbourMap.begin(); it != m_neighbourMap.end(); it++)
  {
    if (it.value().m_expires < now)
    {
      list.append(it.key());
    }
  }
  for (auto it : list)
  {
    removeNeighbour(it);
  }
}

void NetworkLayer::helloNeighbours()
{
  QMutexLocker locker(&m_connectionListMutex);

  for (auto connection : m_connectionList)
  {
    HelloNeighbourPacket request = HelloNeighbourPacket::createRequest(
          connection.connection()->getAddress(), m_address);
    connection.connection()->broadcast(request.asBytes(), request.len());
  }

}

void NetworkLayer::sendNeighboursList()
{
  QMutexLocker locker(&m_neighbourMapMutex);
  QMutexLocker locker2(&m_connectionListMutex);

  NeighbourInfoPacket packet(m_address, m_sequenceNumber++);

  for (auto neighbourInfo : m_neighbourMap)
  {
    qDebug() << "NeighbourInfo:" << neighbourInfo.m_IPAddress
             << neighbourInfo.m_distance;
    packet.append(neighbourInfo.m_IPAddress, neighbourInfo.m_distance);
  }
  BytePtr bytes;
  uint len = packet.toBytes(bytes);
//qDebug() << "BLA";
//qDebug("P: %02x", packet.m_senderAddress);
//qDebug("P: %02x", packet.m_sequenceNumber);
//for (uint i = 0; i < len; i++)
//{
//  qDebug("%02x", bytes.get()[i]);
//}
  for (auto neighbourInfo : m_neighbourMap)
  {
    neighbourInfo.m_connection->send(neighbourInfo.m_MACAddress,
                                     bytes.get(), len);
  }

}

void NetworkLayer::parseFrame(ILLCSublayerPtr connection,
                              const IMACSublayer::Address &address,
                              BytePtr bytes, uint len)
{
  FrameType type = (FrameType) bytes.get()[0];
  switch (type)
  {
    case FrameType::HelloRequest:
      parseHelloRequest(connection, address, bytes, len); break;
    case FrameType::HelloAnswer:
      parseHelloAnswer(connection, address, bytes, len); break;
    case FrameType::NeighbourInfo:
      parseNeighbourList(connection, address, bytes, len); break;
    case FrameType::NeighbourInfoACK:
      parseNeighbourListACK(address, bytes, len); break;
    default: qDebug() << "Unknown frame.";
  };
}

void NetworkLayer::parseHelloRequest(ILLCSublayerPtr connection,
                                     const IMACSublayer::Address &address,
                                     BytePtr bytes, uint len)
{
  qDebug() << "HelloRequest:" << this << address;
  if (len < HelloNeighbourPacket::len())
  {
    return;
  }
  HelloNeighbourPacket answer = HelloNeighbourPacket::createAnswer(
        HelloNeighbourPacket::fromBytes(bytes), connection->getAddress(),
        m_address);
  connection->send(address, answer.asBytes(), answer.len());
}

void NetworkLayer::parseHelloAnswer(ILLCSublayerPtr connection,
                                    const IMACSublayer::Address &address,
                                    BytePtr bytes, uint len)
{
  qDebug() << "HelloAnswer:" << this << address;
  if (len < HelloNeighbourPacket::len())
  {
    return;
  }
  HelloNeighbourPacket answer = HelloNeighbourPacket::fromBytes(bytes);
  QMutexLocker locker(&m_neighbourMapMutex);
  addNeighbour(NeighbourInfo(
                 address, connection, answer.m_IPAddress,
                 answer.m_answerSent - answer.m_sent));
}

void NetworkLayer::parseNeighbourList(ILLCSublayerPtr connection,
                                      const IMACSublayer::Address &address,
                                      BytePtr bytes, uint len)
{
  qDebug() << "Neighbours:" << this << address;
  if (len < NeighbourInfoPacket::headerLength())
  {
    return;
  }
  NeighbourInfoPacket packet = NeighbourInfoPacket::fromBytes(bytes);
//qDebug() << "KVA";
//qDebug("P: %02x", packet.m_senderAddress);
//qDebug("P: %02x", packet.m_sequenceNumber);
//for (uint i = 0; i < len; i++)
//{
//  qDebug("%02x", bytes.get()[i]);
//}
  qDebug() << "Packet info:" << m_address;
  qDebug() << "Sender:" << packet.m_senderAddress;
  qDebug() << "Sequence:" << packet.m_sequenceNumber;
  qDebug() << "Length:" << packet.m_length;
  for (uint i = 0; i < packet.m_length; i++)
  {
    qDebug() << i << packet.m_neighbours[i].m_address
             << packet.m_neighbours[i].m_distance;
  }
  m_routingTable.updateRouterInfo(connection, address, packet);
}

void NetworkLayer::parseNeighbourListACK(
  const IMACSublayer::Address &address,
  BytePtr bytes, uint len)
{
  if (len != NeighbourInfoACKPacket::length())
  {
    return;
  }
  NeighbourInfoACKPacket packet = NeighbourInfoACKPacket::fromBytes(bytes);
  m_routingTable.checkACKPackage(address, packet);
}
