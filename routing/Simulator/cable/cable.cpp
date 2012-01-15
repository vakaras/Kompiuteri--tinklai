#include "cable.h"
#include <cstdlib>

Cable::Cable(double errorRate, ulong sleepTime, QObject *parent) :
    QObject(parent), m_errorRate(errorRate), m_process(this, sleepTime)
{
  m_process.start();
}

Cable::~Cable()
{
  m_process.stop();
  lockAll();
  deleteUnused();
  Q_ASSERT(m_connectionPoints.isEmpty());
                                        // Cabel must be deleted after
                                        // all clients.
}

void Cable::lockAll()
{
  m_connectionPointsMutex.lock();
  for (auto connectionPoint : m_connectionPoints)
  {
    connectionPoint->lock();
  }
}

void Cable::unlockAll()
{
  for (auto connectionPoint : m_connectionPoints)
  {
    connectionPoint->unlock();
  }
  m_connectionPointsMutex.unlock();
}

void Cable::deleteUnused()
{
  ConnectionPointList toDelete;
  for (auto connectionPoint = m_connectionPoints.begin();
       connectionPoint != m_connectionPoints.end(); connectionPoint++)
  {
    if (connectionPoint->unique())
    {
      toDelete.append(*connectionPoint);
    }
  }
  for (auto connectionPoint : toDelete)
  {
    m_connectionPoints.removeOne(connectionPoint);
  }
}

void Cable::processCycle()
{
  lockAll();
  deleteUnused();
  Bit bit = 0;
  uint writersCount = 0;
  for (auto connectionPoint : m_connectionPoints)
  {
    if (!connectionPoint->isEmpty())
    {
      bit += connectionPoint->pop();
      writersCount++;
    }
  }
  bit ^= (qrand() <= RAND_MAX * m_errorRate);
  for (auto connectionPoint : m_connectionPoints)
  {
    if (!connectionPoint->isEmpty())
    {
      connectionPoint->setCollision(writersCount > 1);
    }
    if (writersCount > 0)
    {
      connectionPoint->push(bit);
    }
    connectionPoint->notify(writersCount == 0);
  }
  unlockAll();
}

Cable::ConnectionPointPtr Cable::createConnectionPoint()
{
  QMutexLocker locker(&m_connectionPointsMutex);
  ConnectionPointPtr p(new ConnectionPoint());
  m_connectionPoints.append(p);
  return p;
}
