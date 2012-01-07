#include "networklayer/networklayer.h"

NetworkLayer::NetworkLayer(INetworLayer::Address address, QObject *parent) :
    QObject(parent), m_address(address), m_calculationExpires(0),
    m_routingProcess(this)
{
  qDebug() << "ST";
  m_routingProcess.start();
}

NetworkLayer::~NetworkLayer()
{
  QMutexLocker locker(&m_connectionListMutex);
  m_connectionList.clear();
  m_routingProcess.stop();
}

void NetworkLayer::append(ILLCSublayerPtr connection)
{
  QMutexLocker locker(&m_connectionListMutex);
  QMutexLocker locker2(&m_calculationExpiresMutex);

  m_connectionList.append(connection);
  m_calculationExpires = 0;
}

bool NetworkLayer::remove(ILLCSublayerPtr connection)
{
  QMutexLocker locker(&m_connectionListMutex);
  QMutexLocker locker2(&m_calculationExpiresMutex);

  m_calculationExpires = 0;
  return m_connectionList.removeOne(connection);
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
