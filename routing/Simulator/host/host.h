#ifndef HOST_H
#define HOST_H

#include <functional>
#include <utils/genericthread.h>
#include <QMap>
#include <cable/cable.h>
#include <macsublayer/macsublayer.h>
#include <llcsublayer/llcsublayer.h>
#include <networklayer/networklayer.h>
#include <transportlayer/transportlayer.h>


class Host : public GenericThread
{

  Q_OBJECT

private:

  _T std::function<void (Host *)>     VoidFunction;

  _M VoidFunction             m_function;
  _M void                     run();

  _T QMap<uint, IMACSublayerPtr>  MACMap;
  _T QMap<uint, ILLCSublayerPtr>  LLCMap;

  _M INetworkLayer::Address       m_address;
  _M MACMap                       m_macs;
  _M LLCMap                       m_llcs;
  _M NetworkLayer                 m_networkLayer;
  _M TransportLayer               m_transportLayer;
  _M uint                         m_lastConnectionId;

public:

  explicit Host(INetworkLayer::Address address,
                VoidFunction function, QObject *parent = 0);
  _M Vacuum           ~Host();
  /** Connect to cable. Returns connection id. */
  _M uint             connect(Cable &cable, IMACSublayer::Address address);
  /** Disconnects from cable. */
  _M bool             disconnect(uint connectionId);

  _M TransportLayer*  transportLayer();
};

#endif // HOST_H
