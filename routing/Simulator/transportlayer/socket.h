#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>
#include <interfaces/ITransportLayer.h>
#include <interfaces/INetworkLayer.h>
#include <transportlayer/tcppacket.h>


extern uint MAX_SEGMENT_SIZE;
extern uint MAX_BUFFER_SIZE;
extern uint MAX_SEGMENT_AMOUNT;


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

  /** Sequence number of next byte, which host is expecting to get.
    (This is ACK for other host.)
    */
  _M uint                       m_destinationSequence;
  /** Sequence number of next byte, which is going to be sent. When
    sending, first send, then increase.
    */
  _M uint                       m_sourceSequence;

  _M bool                       m_connected;

  _M QMutex                     m_socketMutex;

  _M INetworkLayer*             m_network;

    // For connection establishing.
  _M QWaitCondition             m_connectWaitCondition;

  _M bool                       send(TCPPacket packet);

    // For sending data.

  _M QMutex                     m_sendMutex;
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

  _M bool                       sendBurst(const Byte *bytes, uint len);

    // For receiving data.

  // Data for application.
  _M ByteMap                    m_readBuffer;
  /** Just bytes from interval
    [m_readBufferLowerBound; m_readBufferLowerBound + MAX_BUFFER_SIZE]
    are accepted into m_readBuffer.
    */
  _M uint                       m_readBufferLowerBound;
  _M QWaitCondition             m_readBufferWaitCondition;
  /// There exists packet, which is needed to be acknowledged.
  _M bool                       m_needToAck;

public:

  explicit Socket(INetworkLayer *network,
                  ITransportLayer::Address destinationAddress,
                  ITransportLayer::Port destinationPort,
                  ITransportLayer::Port sourcePort,
                  Type socketType,
                  QObject *parent = 0);

  _M bool   send(const Byte *bytes, uint len);
  _M uint   receive(BytePtr &bytes, ulong time=ULONG_MAX);
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
