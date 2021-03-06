#include "connectionpoint.h"

ConnectionPoint::ConnectionPoint(QObject *parent) :
    QObject(parent), m_bufferOut(0), m_empty(true),
    m_collision(true), m_mediumWasFree(0)
{
}

bool ConnectionPoint::write(Bit bit)
{
  QMutexLocker locker(&m_mutex);
  m_bufferOut = bit;
  m_empty = false;
  m_writerWaitCondition.wait(&m_mutex);
  return !m_collision;
}

Bit ConnectionPoint::read()
{
  bool timeOuted = false;
  return read(ULONG_MAX, &timeOuted);
}

Bit ConnectionPoint::read(ulong time, bool *timeOuted)
{
  QMutexLocker locker(&m_mutex);
  while (m_bufferIn.size() == 0)
  {
    if (!m_readerWaitCondition.wait(&m_mutex, time))
    {
      *timeOuted = true;
      return 0;
    }
  }
  *timeOuted = false;
  return m_bufferIn.dequeue();
}

void ConnectionPoint::reconnect()
{
  QMutexLocker locker(&m_mutex);
  m_empty = true;
  m_bufferIn.clear();
}

void ConnectionPoint::wait(ulong number)
{
  QMutexLocker locker(&m_mutex);
  while (number > 0)
  {
    m_cableWaitCondition.wait(&m_mutex);
    number--;
  }
}

bool ConnectionPoint::isFree(uint cycles)
{
  QMutexLocker locker(&m_mutex);
  return m_mediumWasFree >= cycles;
}

void ConnectionPoint::lock()
{
  m_mutex.lock();
}

void ConnectionPoint::unlock()
{
  m_mutex.unlock();
}

bool ConnectionPoint::isEmpty()
{
  return m_empty;
}

Bit ConnectionPoint::pop()
{
  return m_bufferOut;
}

void ConnectionPoint::setCollision(bool collision)
{
  m_collision = collision;
  m_empty = true;
  m_writerWaitCondition.wakeOne();
}

void ConnectionPoint::push(Bit bit)
{
  m_bufferIn.enqueue(bit);
  m_readerWaitCondition.wakeOne();
}

void ConnectionPoint::notify(bool free)
{
  if (free)
    m_mediumWasFree++;
  else
    m_mediumWasFree = 0;
  m_cableWaitCondition.wakeOne();
}
