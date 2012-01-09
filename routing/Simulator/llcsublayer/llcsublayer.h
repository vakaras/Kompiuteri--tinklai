#ifndef LLCSUBLAYER_H
#define LLCSUBLAYER_H

#include <QObject>
#include <interfaces/ILLCSublayer.h>
#include <interfaces/IMACSublayer.h>


class LLCSublayer : public QObject, public ILLCSublayer
{

  Q_OBJECT

private:

  _M IMACSublayer*  m_mac;

public:

  explicit LLCSublayer(IMACSublayer *mac,
                       QObject *parent = 0);

  _M bool     send(Address, Byte *bytes, uint len);
  _M bool     broadcast(Byte *bytes, uint len);
  _M uint     receive(Address &address,
                      BytePtr &bytes, ulong time=ULONG_MAX);
  _M void     reconnect();
  _M Address  getAddress();

};

#endif // LLCSUBLAYER_H
