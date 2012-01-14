#include "routingtableprocess.h"
#include <networklayer/routingtable.h>

RoutingTableProcess::RoutingTableProcess(
  RoutingTable *table, QObject *parent) :
    GenericThread(parent), m_table(table)
{
}

void RoutingTableProcess::stop()
{
  changeGo(false);
  wake();
  QThread::wait();
}

void RoutingTableProcess::wake()
{
  m_waitCondition.wakeOne();
}

void RoutingTableProcess::run()
{
  QMutexLocker locker(&m_goMutex);
  while (m_go)
  {
    m_waitCondition.wait(&m_goMutex, m_table->forwardPackages());
  }
}
