#include "unidirectionalcable.h"
#include <cstdlib>

UnidirectionalCable::UnidirectionalCable(double errorRate, QObject *parent) :
    QObject(parent), m_errorRate(errorRate)
{
}

Byte UnidirectionalCable::read()
{
  QMutexLocker locker(&m_mutex);
  m_semaphore.acquire();
  return m_buffer.dequeue();
}

void UnidirectionalCable::write(Byte byte)
{
  QMutexLocker locker(&m_mutex);
  Byte result = 0;
  for (int i = 0; i < 8; i++)
  {
    Byte bit = (byte >> i) & 1;
    bit ^= (int) (qrand() >= RAND_MAX * m_errorRate);
    result |= bit << i;
  }
  m_buffer.enqueue(byte);
  m_semaphore.release();
}
