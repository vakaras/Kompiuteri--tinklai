#ifndef ROUTINGPROCESS_H
#define ROUTINGPROCESS_H

#include <QThread>
#include <QMutex>
#include <types.h>


class NetworkLayer;

class RoutingProcess : public QThread
{

  Q_OBJECT

private:

  _M NetworkLayer*          m_layer;
  _M bool                   m_go;
  _M QMutex                 m_goMutex;

  _M void                   run();
  _M void                   changeGo(bool go);

public:

  explicit RoutingProcess(NetworkLayer *layer, QObject *parent = 0);
  _M void                   start();
  _M void                   stop();

};

#endif // ROUTINGPROCESS_H
