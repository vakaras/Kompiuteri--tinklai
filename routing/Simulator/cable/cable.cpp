#include "cable.h"
#include <cstdlib>

Cable::Cable(double errorRate, ulong sleepTime, QObject *parent) :
    QObject(parent), m_errorRate(errorRate), m_process(this, sleepTime)
{
  m_process.start();
}

Cable::~Cable()
{
  while (!m_connectionPoints.isEmpty())
  {
    ConnectionPointPtr p = m_connectionPoints.first();
    m_connectionPoints.removeFirst();
    delete p;
  }
  m_process.stop();
}

void Cable::lockAll()
{
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
}

void Cable::processCycle()
{
  lockAll();
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

ConnectionPoint* Cable::createConnectionPoint()
{
  ConnectionPointPtr p = new ConnectionPoint();
  m_connectionPoints.append(p);
  return p;
}
