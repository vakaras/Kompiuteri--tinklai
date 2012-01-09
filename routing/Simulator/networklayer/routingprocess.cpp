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
  m_layer->m_routingProcessWaitCondition.wait(
        &m_layer->m_routingProcessMutex, 500);
  while (m_go)
  {
    qDebug() << "Routing Started!";
    m_layer->removeOldNeighbours();
    m_layer->helloNeighbours();
    qDebug() << "Waiting answers." << m_layer;
    m_layer->m_routingProcessWaitCondition.wait(
          &m_layer->m_routingProcessMutex, 3000);
    qDebug() << "Looking for answers." << m_layer;
    m_layer->sendNeighboursList();
    m_layer->setCalculationExpires(
          QDateTime::currentMSecsSinceEpoch() + ROUTES_UPDATE_PERIOD);
    qDebug() << "Sleeping." << m_layer;
    m_layer->m_routingProcessWaitCondition.wait(
          &m_layer->m_routingProcessMutex,
          (ulong) qMax(m_layer->calculationExpires()
                       - QDateTime::currentMSecsSinceEpoch(), 0ll));
  }
}
