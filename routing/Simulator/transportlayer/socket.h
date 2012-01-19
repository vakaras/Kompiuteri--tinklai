#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <interfaces/ITransportLayer.h>
#include <interfaces/INetworkLayer.h>
#include <transportlayer/tcppacket.h>


#define MAX_SEGMENT_SIZE (1u << 8)
#define MAX_BUFFER_SIZE (1u << 14)
#define MAX_SEGMENT_AMOUNT (MAX_BUFFER_SIZE/MAX_SEGMENT_SIZE)


class Socket : public QObject, public ISocket
{

  Q_OBJECT

public:

  // Sequence number -> Byte.
  _T QMap<uint, Byte>           ByteMap;
  _E class Type
  {
    Server,
    Client
  };

private:

  _M Type                       m_socketType;
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

    // For sending data.

  _M uint                       m_rtt;
  _M QWaitCondition             m_senderWaitCondition;
  /// Until when sender should wait for ack.
  _M MSec                       m_senderTimeoutMoment;
  /// How much bytes other side agree to accept.
  _M uint                       m_destinationWindowSize;
  /// First unacknowledged byte sequence number.
  _M uint                       m_ackSequence;

  _M uint                       m_congestionWindowSize;
  _M uint                       m_threshold;

  _M bool                       sendBurst(Byte *bytes, uint len);

    // For receiving data.

  // Data for application.
  _M ByteMap                    m_readBuffer;

public:

  explicit Socket(INetworkLayer *network,
                  ITransportLayer::Address destinationAddress,
                  ITransportLayer::Port destinationPort,
                  ITransportLayer::Port sourcePort,
                  Type socketType,
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
  _M void   finalizeConnecting(TCPPacket packet);
  _M void   setDestinationSequence(uint sequence);

};

typedef std::shared_ptr<Socket>   SocketPtr;

#endif // SOCKET_H
