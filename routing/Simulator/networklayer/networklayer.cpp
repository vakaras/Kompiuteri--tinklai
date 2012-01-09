#include "networklayer/networklayer.h"
#include <networklayer/helloneighbourpacket.h>
#include <QDateTime>

NetworkLayer::NetworkLayer(INetworLayer::Address address, QObject *parent) :
    QObject(parent), m_address(address), m_calculationExpires(0),
    m_routingProcess(this)
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

bool NetworkLayer::remove(ILLCSublayerPtr connection)
{
  QMutexLocker locker(&m_connectionListMutex);
  QMutexLocker locker2(&m_calculationExpiresMutex);

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
    m_neighbourMap.remove(it);
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
  qDebug() << "Sending answer.";
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
  qDebug() << "Parsing answer.";
  HelloNeighbourPacket answer = HelloNeighbourPacket::fromBytes(bytes);
  QMutexLocker locker(&m_neighbourMapMutex);
  m_neighbourMap[answer.m_MACAddress] = NeighbourInfo(
        address, connection, answer.m_IPAddress,
        answer.m_answerSent - answer.m_sent);
}
