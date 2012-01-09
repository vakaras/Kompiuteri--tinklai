#ifndef NETWORKREADER_H
#define NETWORKREADER_H

#include <QMutex>
#include <QThread>
#include <interfaces/ILLCSublayer.h>


class NetworkLayer;

class NetworkReader : public QThread
{

  Q_OBJECT

private:

  _M NetworkLayer*          m_layer;
  _M ILLCSublayerPtr        m_connection;
  _M bool                   m_go;
  _M QMutex                 m_goMutex;

  _M void                   run();
  _M void                   changeGo(bool go);

public:

  explicit NetworkReader(NetworkLayer *layer, ILLCSublayerPtr connection,
                         QObject *parent = 0);
  _M Vacuum                 ~NetworkReader();
  _M void                   start();
  _M void                   stop();
};

typedef std::shared_ptr<NetworkReader>  NetworkReaderPtr;

#endif // NETWORKREADER_H
