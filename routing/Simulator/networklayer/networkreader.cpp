#include "networkreader.h"
#include <networklayer/networklayer.h>

NetworkReader::NetworkReader(NetworkLayer *layer,
                             ILLCSublayerPtr connection,
                             QObject *parent):
    QThread(parent), m_layer(layer), m_connection(connection), m_go(false)
{
}

NetworkReader::~NetworkReader()
{
  stop();
}

void NetworkReader::start()
{
  changeGo(true);
  qDebug() << "Start." << m_go;
  QThread::start();
}

void NetworkReader::stop()
{
  qDebug() << "Stop.";
  changeGo(false);
  QThread::wait();
}

void NetworkReader::changeGo(bool go)
{
  QMutexLocker locker(&m_goMutex);
  m_go = go;
}

void NetworkReader::run()
{
  m_goMutex.lock();
  while (m_go)
  {
    m_goMutex.unlock();
    ILLCSublayer::Address address;
    BytePtr bytes;
    uint len = m_connection->receive(address, bytes, 200);
    if (len)
    {
      m_layer->parseFrame(m_connection, address, bytes, len);
    }
    m_goMutex.lock();
  }
  m_goMutex.unlock();
}
