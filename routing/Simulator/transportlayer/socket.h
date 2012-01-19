#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <interfaces/ITransportLayer.h>
#include <interfaces/INetworkLayer.h>
#include <transportlayer/tcppacket.h>


class Socket : public QObject, public ISocket
{

  Q_OBJECT

private:

  _M ITransportLayer::Address   m_destinationAddress;
  _M ITransportLayer::Port      m_destinationPort;
  _M ITransportLayer::Port      m_sourcePort;

  _M uint                       m_destinationSequence;
  _M uint                       m_sourceSequence;

  _M bool                       m_connected;

  _M QMutex                     m_socketMutex;

  _M INetworkLayer*             m_network;

  // For connection establishing.
  _M QWaitCondition             m_connectWaitCondition;

  _M bool                       send(TCPPacket packet);

public:

  explicit Socket(INetworkLayer *network,
                  ITransportLayer::Address destinationAddress,
                  ITransportLayer::Port destinationPort,
                  ITransportLayer::Port sourcePort,
                  QObject *parent = 0);

  _M bool   send(Byte *bytes, uint len);
  _M uint   receive(BytePtr &bytes, ulong time=ULONG_MAX);
  _M void   disconnect();
  _M bool   isConnected();

  _M bool   connect();
  /// Sends response to connecting client. (Second handshake.)
  _M void   sendConnectResponse();
  _M void   parseSegment(ITransportLayer::Address address,
                         TCPPacket packet);
  /// Client sends last ACK to server and marks connection as established.
  _M void   finalizeConnecting();
  _M void   setDestinationSequence(uint sequence);
  _M void   setConnected(bool connected);

};

typedef std::shared_ptr<Socket>   SocketPtr;

#endif // SOCKET_H
