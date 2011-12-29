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
  _T QQueue<Bit>     DataBuffer;

  /** Probability, that bit will be inversed. */
  _M double           m_errorRate;
  /** Probability, that bit will be lost. */
  _M double           m_lostRate;

  _M QSemaphore       m_semaphore;
  _X QMutex           m_mutex;
  _M DataBuffer       m_buffer;

public:

  explicit UnidirectionalCable(
    double errorRate = 0.0, double lostRate = 0.0, QObject *parent = 0);
  _M Bit              read();
  _M void             write(Bit byte);


signals:

public slots:

};

#endif // UNIDIRECTIONALCABLE_H
