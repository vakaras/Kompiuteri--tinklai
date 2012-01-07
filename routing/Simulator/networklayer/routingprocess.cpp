#include "routingprocess.h"
#include <QDateTime>
#include <networklayer/networklayer.h>

RoutingProcess::RoutingProcess(NetworkLayer *layer, QObject *parent) :
    QThread(parent), m_layer(layer), m_go(false)
{
}

void RoutingProcess::start()
{
  changeGo(true);
  QThread::start();
}

void RoutingProcess::stop()
{
  changeGo(false);
  m_layer->m_routingProcessWaitCondition.wakeOne();
  QThread::wait();
}

void RoutingProcess::changeGo(bool go)
{
  QMutexLocker locker(&m_goMutex);
  m_go = go;
}

void RoutingProcess::run()
{
  QMutexLocker locker(&m_layer->m_routingProcessMutex);
  while (m_go)
  {
    qDebug() << "RoutingProcess: Sleep";
    m_layer->m_routingProcessWaitCondition.wait(
          &m_layer->m_routingProcessMutex,
          (ulong) qMax(QDateTime::currentMSecsSinceEpoch()
                       - m_layer->calculationExpires(), 0ll));
    qDebug() << "RoutingProcess: set expires";
    m_layer->setCalculationExpires(
          QDateTime::currentMSecsSinceEpoch() + ROUTES_UPDATE_PERIOD);
  }
}
