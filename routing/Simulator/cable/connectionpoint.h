#ifndef CONNECTIONPOINT_H
#define CONNECTIONPOINT_H

#include <QObject>
#include <QList>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <interfaces/IPhysicalConnection.h>


class TestCable;

class ConnectionPoint : public QObject, public IPhysicalConnection
{

  Q_OBJECT

private:

  /// Buffer in which client writes.
  _M Bit            m_bufferOut;
  _M bool           m_empty;
  _M bool           m_collision;
  _M QWaitCondition m_writerWaitCondition;
  /// Buffer from which client reads.
  _M QQueue<Bit>    m_bufferIn;
  _M QWaitCondition m_readerWaitCondition;
  /// For locking buffers.
  _M QMutex         m_mutex;

  /// For waiting.
  _M QWaitCondition m_cableWaitCondition;
  _M uint           m_mediumWasFree;

  _F class          TestCable;

public:

  explicit ConnectionPoint(QObject *parent = 0);

  // Interface for client.
  _M bool       write(Bit bit);
  _M Bit        read(ulong time=ULONG_MAX, bool *timeOuted=NULL);
  _M void       reconnect();
  _M void       wait(ulong number=1);
  _M bool       isFree(uint cycles=1);

  // Interface for cable.
  _M void       lock();
  _M void       unlock();
  /// Returns true, if m_bufferOut is empty.
  _M bool       isEmpty();
  /// Returns bit from clients out buffer.
  _M Bit        pop();
  /// Cable pass true, if collision was detected. Deletes bit from clients
  /// out buffer.
  _M void       setCollision(bool collision);
  /// Adds bit to client in buffer.
  _M void       push(Bit bit);
  /** Notify about the end of cycle.
    @param free – if medium this cycle was free.
    */
  _M void       notify(bool free);

};

#endif // CONNECTIONPOINT_H
