#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QMap>
#include <cable/cable.h>
#include <macsublayer/macsublayer.h>
#include <llcsublayer/llcsublayer.h>
#include <networklayer/networklayer.h>


class TestNetworkLayer;

class Router : public QObject, public INetworLayer
{

  Q_OBJECT

private:

  _F class                        TestNetworkLayer;
  _T QMap<uint, IMACSublayerPtr>  MACMap;
  _T QMap<uint, ILLCSublayerPtr>  LLCMap;

  _M INetworLayer::Address        m_address;
  _M MACMap                       m_macs;
  _M LLCMap                       m_llcs;
  _M NetworkLayer                 m_networkLayer;
  _M uint                         m_lastConnectionId;

public:

  explicit Router(INetworLayer::Address address, QObject *parent = 0);

  _M Vacuum   ~Router();

  /** Connect to cable. Returns connection id. */
  _M uint     connect(Cable &cable, IMACSublayer::Address address);

  /** Disconnects from cable. */
  _M bool     disconnect(uint connectionId);

  _M Address  address();

  _M bool     send(Address address, Byte *bytes, uint len);
  _M uint     receive(Address &address, BytePtr &bytes,
                    ulong time=ULONG_MAX);
  _M void     restart();
};

#endif // ROUTER_H
