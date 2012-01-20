#include "genericthread.h"

GenericThread::GenericThread(QObject *parent) :
    QThread(parent), m_go(false)
{
}

void GenericThread::start()
{
  changeGo(true);
  QThread::start();
}

void GenericThread::stop()
{
  changeGo(false);
  QThread::wait();
}

void GenericThread::changeGo(bool go)
{
  QMutexLocker locker(&m_goMutex);
  m_go = go;
}

bool GenericThread::go()
{
  QMutexLocker locker(&m_goMutex);
  return m_go;
}
