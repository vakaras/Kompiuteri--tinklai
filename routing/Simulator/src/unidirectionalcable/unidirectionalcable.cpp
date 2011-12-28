#include "unidirectionalcable.h"
#include <cstdlib>

UnidirectionalCable::UnidirectionalCable(double errorRate, QObject *parent) :
    QObject(parent), m_errorRate(errorRate)
{
}

Byte UnidirectionalCable::read()
{
  m_semaphore.acquire();
  QMutexLocker locker(&m_mutex);
  return m_buffer.dequeue();
}

void UnidirectionalCable::write(Byte byte)
{
  QMutexLocker locker(&m_mutex);
  Byte result = 0;
  for (int i = 0; i < 8; i++)
  {
    Byte bit = (byte >> i) & 1;
    int rand = qrand();
    bit ^= (int) (rand <= RAND_MAX * m_errorRate);
    result |= bit << i;
  }
  m_buffer.enqueue(result);
  m_semaphore.release();
}
