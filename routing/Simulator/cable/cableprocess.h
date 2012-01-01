#ifndef CABLEPROCESS_H
#define CABLEPROCESS_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <types.h>

class Cable;

class CableProcess : public QThread
{

  Q_OBJECT

private:

  _M Cable*           m_cable;

  // Fields for controling thread execution.
  _M bool             m_go;
  /// How long thread should sleep between cycles.
  _M ulong            m_sleepTime;
  _M QMutex           m_mutex;
  _M QWaitCondition   m_waitCondition;

protected:

  _M void       run();

public:

  explicit CableProcess(Cable *cable, ulong sleepTime=0,
                        QObject *parent = 0);
  _M void       start();
  _M void       stop();
};

#endif // CABLEPROCESS_H
