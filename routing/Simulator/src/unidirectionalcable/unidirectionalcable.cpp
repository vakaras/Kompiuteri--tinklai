#include "unidirectionalcable.h"
#include <cstdlib>

UnidirectionalCable::UnidirectionalCable(
  double errorRate, double lostRate, QObject *parent) :
    QObject(parent), m_errorRate(errorRate), m_lostRate(lostRate)
{
}

Bit UnidirectionalCable::read(ulong time, bool *timeOuted)
{
  QMutexLocker locker(&m_mutex);
  while (m_buffer.size() == 0)
  {
    if (!m_waitCondition.wait(&m_mutex, time))
    {
      if (timeOuted)
      {
        *timeOuted = true;
      }
      return 0;
    }
  }
  if (timeOuted)
  {
    *timeOuted = false;
  }
  return m_buffer.dequeue();
}

void UnidirectionalCable::write(Bit bit)
{
  QMutexLocker locker(&m_mutex);
  if (qrand() >= RAND_MAX * m_lostRate)
  {
    bit ^= (qrand() <= RAND_MAX * m_errorRate);
    m_buffer.enqueue(bit);
    m_waitCondition.wakeOne();
  }
  if (qrand() <= RAND_MAX * m_lostRate)
  {
    m_buffer.enqueue(qrand() <= (RAND_MAX >> 1));
    m_waitCondition.wakeOne();
  }
}


void UnidirectionalCable::reset()
{
  QMutexLocker locker(&m_mutex);
  m_semaphore.acquire(m_semaphore.available());
  m_buffer.clear();
}
