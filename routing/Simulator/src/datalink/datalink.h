#ifndef DATALINK_H
#define DATALINK_H


#include <QObject>
#include <QThread>
#include <QList>
#include <src/types.h>
#include <src/interfaces/IPhysicalConnection.h>
#include <src/interfaces/ILink.h>


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



class DataLink : public QObject
{

  Q_OBJECT

private:

  _M IPhysicalConnection    *m_connectionIn;
  _M IPhysicalConnection    *m_connectionOut;
  _M DataLinkReader         m_reader;

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

  _M void             writeFrame(const FrameHeader *header,
                                 const Byte *bytes, uint len);
  _M void             parseFrame(QList<Bit> *buffer);

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
                    QObject *parent = 0);
  _M ~DataLink();
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
