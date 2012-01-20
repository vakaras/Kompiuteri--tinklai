#ifndef GENERICTHREAD_H
#define GENERICTHREAD_H

#include <QThread>
#include <QMutex>
#include <types.h>

class GenericThread : public QThread
{

  Q_OBJECT

protected:

  _M bool                   m_go;
  _M QMutex                 m_goMutex;
  _V void                   changeGo(bool go);

public:

  explicit GenericThread(QObject *parent = 0);
  _V void                   start();
  _V void                   stop();
  _V bool                   go();

};

#endif // GENERICTHREAD_H
