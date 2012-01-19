#include "networklayer/networklayer.h"
#include <networklayer/helloneighbourpacket.h>
#include <networklayer/neighbourinfopacket.h>
#include <networklayer/neighbourinfoackpacket.h>
#include <networklayer/ippacket.h>
#include <QDateTime>

NetworkLayer::NetworkLayer(INetworkLayer::Address address, QObject *parent) :
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

void NetworkLayer::removeNeighbour(INetworkLayer::Address address)
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
  QMutexLocker locker(&m_sendMutex);
  IPPacket packet(m_address, address, bytes, len);
  return m_routingTable.forward(packet);
}

uint NetworkLayer::receive(Address &address, BytePtr &bytes, ulong time)
{
  QMutexLocker locker(&m_readBufferMutex);
  while (m_readBuffer.isEmpty())
  {
    if (!m_readBufferWaitCondition.wait(&m_readBufferMutex, time))
    {
      return 0;
    }
  }
  uint len;
  std::tie(address, bytes, len) = m_readBuffer.first();
  m_readBuffer.removeFirst();
  return len;
}

void NetworkLayer::restart()
{
  // TODO
//QMutexLocker locker4(&m_sendMutex);
//m_routingProcess.stop();
//setCalculationExpires(0);
//QMutexLocker locker1(&m_connectionListMutex);
//QMutexLocker locker2(&m_neighbourMapMutex);
//QMutexLocker locker3(&m_readBufferMutex);
//m_neighbourMap.clear();
//m_sequenceNumber = 0;
//m_readBuffer.clear();
//m_routingTable = RoutingTable(m_address);

//m_routingProcess.start();
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
  if (!list.isEmpty())
  {
    m_routingTable.update();
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
    case FrameType::IP:
      parseIP(bytes, len); break;
    default: qDebug() << "Unknown frame.";
  };
}

void NetworkLayer::parseHelloRequest(ILLCSublayerPtr connection,
                                     const IMACSublayer::Address &address,
                                     BytePtr bytes, uint len)
{
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
//qDebug() << "Packet info:" << m_address;
//qDebug() << "Sender:" << packet.m_senderAddress;
//qDebug() << "Sequence:" << packet.m_sequenceNumber;
//qDebug() << "Length:" << packet.m_length;
//for (uint i = 0; i < packet.m_length; i++)
//{
//  qDebug() << i << packet.m_neighbours[i].m_address
//           << packet.m_neighbours[i].m_distance;
//}
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

void NetworkLayer::parseIP(BytePtr bytes, uint len)
{
  if (len < IPPacket::headerLength())
  {
    return;
  }
  IPPacket packet = IPPacket::fromBytes(bytes);
  qDebug() << "IP Packet:" << packet.m_source << packet.m_destination
           << m_address;
  if (packet.m_destination == m_address)
  {
    QMutexLocker locker(&m_readBufferMutex);
    if (m_readBuffer.size() < MAX_READ_BUFFER_SIZE)
    {
      BytePtr bytes = BytePtr(
            new Byte[packet.m_dataLength], sharedArrayDeleter<Byte>);
      memcpy((void *) bytes.get(), (void *) packet.m_data,
             packet.m_dataLength);
      Address address = packet.m_source;
      uint len = packet.m_dataLength;
      m_readBuffer.append(DataFrame(address, bytes, len));
      m_readBufferWaitCondition.wakeOne();
    }
  }
  else
  {
    m_routingTable.forward(packet);
  }
}
