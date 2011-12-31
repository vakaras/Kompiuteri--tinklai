#include "cable.h"
#include <cstdlib>

Cable::Cable(double errorRate, QObject *parent) :
    QObject(parent), m_errorRate(errorRate)
{
}

void Cable::lockAll()
{
  for (auto connectionPoint : m_connectionPoints)
  {
    connectionPoint.lock();
  }
}

void Cable::unlockAll()
{
  for (auto connectionPoint : m_connectionPoints)
  {
    connectionPoint.unlock();
  }
}

void Cable::processCycle()
{
  lockAll();
  Bit bit = 0;
  uint writersCount = 0;
  for (auto connectionPoint : m_connectionPoints)
  {
    if (!connectionPoint.isEmpty())
    {
      bit += connectionPoint.pop();
      writersCount++;
    }
  }
  bit ^= (qrand() <= RAND_MAX * m_errorRate);
  for (auto connectionPoint : m_connectionPoints)
  {
    if (!connectionPoint.isEmpty())
    {
      connectionPoint.setCollision(writersCount > 1);
    }
    connectionPoint.push(bit);
  }
  unlockAll();
}
