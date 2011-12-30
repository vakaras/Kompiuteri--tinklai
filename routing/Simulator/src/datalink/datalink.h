#ifndef DATALINK_H
#define DATALINK_H


#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include <QQueue>
#include <src/types.h>
#include <src/interfaces/IPhysicalConnection.h>
#include <src/interfaces/ILink.h>

#define INFORMATION_FRAME 0x00


class TestDataLink;
class DataLink;

class DataLinkReader : public QThread
{
  Q_OBJECT

private:

  _E State {
    Search,                             // Searching the beginning of the
                                        // next frame.
    Read                                // Reading frame.
    };

  _M DataLink*        m_link;
  _M bool             m_go;

protected:

  _M void         run();

public:
  explicit DataLinkReader(DataLink *link, QObject *parent = 0) :
    QThread(parent), m_link(link), m_go(true)
  {
  }
  _M void         stop()
  {
    m_go = false;
  }
};

class DataLinkWriter : public QThread
{
  Q_OBJECT

private:

  _M DataLink*        m_link;
  _M bool             m_go;

protected:

  _M void         run();

public:
  explicit DataLinkWriter(DataLink *link, QObject *parent = 0) :
    QThread(parent), m_link(link), m_go(true)
  {
  }
  _M void         stop()
  {
    m_go = false;
  }
};


class DataLink : public QObject, public ILink
{

  Q_OBJECT

private:

  struct FrameHeader
  {
    uint                    address: 8;
    union
    {
      struct
      {
        uint                     frameType: 1;
        uint                     seq: 3;
        uint                     pf: 1;
        uint                     next: 3;
      }  __attribute__((packed)) informationControl;
      struct
      {
        uint                     frameType: 1;
        uint                     zero: 1;
        uint                     type: 2;
        uint                     pf: 1;
        uint                     next: 3;
      }  __attribute__((packed)) supervisoryControl;
    } __attribute__((packed));
  } __attribute__((packed));

  struct Frame
  {
    _Y Byte    *start;
    _M uint    len;
    Frame(): start(0), len(0) {}
    Frame(const Byte *_start, uint _len): start(_start), len(_len) {}
  };

  _M IPhysicalConnection    *m_connectionIn;
  _M IPhysicalConnection    *m_connectionOut;
  _M uint                   m_maxFrameSize;
                                        // In bytes.

  // Reading.
  _M DataLinkReader         m_reader;
  _M uint                   m_maxReceivedDataBufferSize;
                                        // m_maxFrameSize * maxFrameCount
                                        // In bytes.
  _M QMutex                 m_receivedDataBufferMutex;
  _M QWaitCondition         m_receivedDataBufferWaitCondition;
  _M QList<Byte>            m_receivedDataBuffer;

  // Writing.
  _M QMutex                 m_writeFunctionMutex;
                                        // Guarantees that at concrete
                                        // moment only one thread tries
                                        // to write.
  _M QQueue<Frame>          m_frameQueue;
  _M QMutex                 m_frameQueueMutex;
  _M QWaitCondition         m_frameQueueWaitCondition;
  _M DataLinkWriter         m_writer;

  // Writing.
  /**
    Writes frame into “cabel”.
    */
  _M void             writeFrame(const FrameHeader *header,
                                 const Byte *bytes, uint len);

  // Reading.
  /**
    Extracts frame information from buffer. Makes basic check.
    */
  _M void             parseFrame(QList<Bit> *buffer);
  /**
    If frame is information, saves its data into buffer and sends response.
    */
  _M void             saveFrame(const FrameHeader *header,
                                const Byte *bytes, uint len);


  /*

  _M bufferSize
  _M buffer

  void    sendFrame(Frame frame);
  Frame   receiveFrame();
  */

  _F class TestDataLink;
  _F class DataLinkReader;

public:

  explicit DataLink(IPhysicalConnection *connectionIn,
                    IPhysicalConnection *connectionOut,
                    uint maxFrameSize = 256,
                    uint maxFrameCount = 16,
                    QObject *parent = 0);
  _M Vacuum   ~DataLink();
  _M bool     write(const Byte *bytes, uint len);
  _M int      read(Byte *bytes, uint maxlen, ulong time=ULONG_MAX);
  _M void     reset();

  /*

    Pradžia:

    1.  lastID = 0 – išsiųstų kadrų unikalūs ID.
    2.  frameSize = ? – kadro dydis baitais.
    3.  frameCount = 0 – paskutinio išsiųsto paketo dydis kadrais.
    4.  framesInfoList – dinaminis sąrašas, kuriame saugoma informacija
        apie kadrus, kurie buvo išsiųsti, bet kurie dar nėra patvirtinti,
        kaip gauti. Saugoma informacija: kadro ID, išsiuntimo laikas
        (seniau išsiųsti kadrai yra visada pradžioje).
    5.  framesBuffer – dinaminis sąrašas, kuriame saugomi kadrai, kurie
        dar nėra atiduoti į „viršų“.

    Išsiuntimas (vykdo siuntėjo gija):

    1.  Užrakina siuntimo funkciją (su muttex).
    2.  Susiskaičiuoja kadrų skaičių frameCount;
    3.  Išsiunčia kadrus ir info sudeda į framesInfoList.
    4.  Patvirtintus kadrus ištrina iš framesInfoList.
    5.  Kai framesInfoList.size == 0, grąžina true.

    Gavimas (vykdo atskira šio lygio gija):

    1.  Nusiskaito bitą ir įrašo į kadro buferį.
    2.  Jei būsena:

      *   kadro gavimas:

        *   CRC-16 galima paskaičiuoti su qChecksum()
        *   jei sėkmingai gautas meta-kadras, tai apie tai praneša
            siuntėjui;
        *   jei sėkmingai gautas informacinis kadras, tai jį
            išsaugo į buferį ir išsiunčia meta-kadrą.
        *   TODO: klaidos…

      *   TODO: kadro pradžios paieška…

    */
  //_V bool write(const Byte *bytes, int len) = 0;

signals:

public slots:

};

#endif // DATALINK_H
