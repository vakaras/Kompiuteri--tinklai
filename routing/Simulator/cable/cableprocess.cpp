#include "cableprocess.h"
#include "cable/cable.h"

CableProcess::CableProcess(Cable *cable, ulong sleepTime, QObject *parent) :
    QThread(parent), m_cable(cable), m_go(false), m_sleepTime(sleepTime)
{
}

void CableProcess::run()
{
  QMutexLocker locker(&m_mutex);
  while (m_go)
  {
    m_waitCondition.wait(&m_mutex, m_sleepTime);
    m_cable->processCycle();
  }
}

void CableProcess::start()
{
  m_mutex.lock();
  m_go = true;
  m_mutex.unlock();
  QThread::start();
  PLOG("Started.");
}

void CableProcess::stop()
{
  m_mutex.lock();
  m_go = false;
  m_mutex.unlock();
  m_waitCondition.wakeOne();
  wait();
  PLOG("Stopped.");
}
