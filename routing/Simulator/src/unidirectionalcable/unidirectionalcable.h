#ifndef UNIDIRECTIONALCABLE_H
#define UNIDIRECTIONALCABLE_H

#include <QObject>
#include <QQueue>
#include <QSemaphore>
#include <QMutex>
#include <src/interfaces/IPhysicalConnection.h>

class UnidirectionalCable : public QObject, public IPhysicalConnection
{

    Q_OBJECT

private:
  _T QQueue<Byte>     DataBuffer;

  _M double           m_errorRate;
  _M QSemaphore       m_semaphore;
  _X QMutex           m_mutex;
  _M DataBuffer       m_buffer;

public:

  explicit UnidirectionalCable(double errorRate = 0.0, QObject *parent = 0);
  _M Byte             read();
  _M void             write(Byte byte);


signals:

public slots:

};

#endif // UNIDIRECTIONALCABLE_H
