#ifndef MACSUBLAYER_H
#define MACSUBLAYER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QMutex>
#include <QWaitCondition>
#include <interfaces/IPhysicalConnection.h>
#include <interfaces/IMACSublayer.h>
#include <macsublayer/macsublayerframe.h>
#include <macsublayer/macreader.h>


class MACSublayer : public QObject, public IMACSublayer
{

  Q_OBJECT

private:

  /// Node MAC address.
  _M IMACSublayer::Address  m_address;

  _M IPhysicalConnectionPtr m_connection;

  // For sending data.
  _M QMutex                 m_writeMethodMutex;

  /** Tries to flush byte sequence (frame) into medium. Does “bit-stuffing”.
    @returns false if collision occurs.
    */
  _M bool                   flushBytes(BytePtr bytes, uint len);

  /** From frame forms byte sequence ready for flushing into medium.
    @returns formed byte sequence length.
    */
  _M uint                   formFrame(MACFrame *frame, BytePtr &bytes);

  /** Sends frame. */
  _M bool                   sendFrame(MACFrame *frame);

  // For receiving data.
  _T QList<MACFrame>        FrameList;
  _M FrameList              m_readBuffer;
  _M QMutex                 m_readBufferMutex;
  _M QWaitCondition         m_readBufferWaitCondition;
  _M MACReader              m_reader;

  /**
    Stores frame into read buffer.
    */
  _M bool                   saveFrame(MACFrame frame);

  /**
    Stores byte sequence as frame into read buffer.
    */
  _M bool                   save(Byte *bytes, uint len);

  _M void                   clearReadBuffer();

public:

  /**
    Saves received data. (This method should be called by read thread.)
    */
  _M bool                   save(BitList bits);

  explicit MACSublayer(IMACSublayer::Address address,
                       IPhysicalConnectionPtr connection,
                       QObject *parent = 0);
  _M Vacuum   ~MACSublayer();

  _M bool     write(IMACSublayer::Address address, Byte *bytes, uint len);
  _M uint     read(IMACSublayer::Address &address, BytePtr &bytes,
                   ulong time=ULONG_MAX);
  _M void     reconnect();

};

#endif // MACSUBLAYER_H
