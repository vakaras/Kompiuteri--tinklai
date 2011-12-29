#include "unidirectionalcable.h"
#include <cstdlib>

UnidirectionalCable::UnidirectionalCable(
  double errorRate, double lostRate, QObject *parent) :
    QObject(parent), m_errorRate(errorRate), m_lostRate(lostRate)
{
}

Bit UnidirectionalCable::read()
{
  m_semaphore.acquire();
  QMutexLocker locker(&m_mutex);
  return m_buffer.dequeue();
}

void UnidirectionalCable::write(Bit bit)
{
  QMutexLocker locker(&m_mutex);
  if (qrand() >= RAND_MAX * m_lostRate)
  {
    bit ^= (qrand() <= RAND_MAX * m_errorRate);
    m_buffer.enqueue(bit);
    m_semaphore.release();
  }
  if (qrand() <= RAND_MAX * m_lostRate)
  {
    m_buffer.enqueue(qrand() <= (RAND_MAX >> 1));
    m_semaphore.release();
  }
}


void UnidirectionalCable::reset()
{
  QMutexLocker locker(&m_mutex);
  m_semaphore.acquire(m_semaphore.available());
  m_buffer.clear();
}
