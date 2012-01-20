#ifndef ITRANSPORTLAYER_H
#define ITRANSPORTLAYER_H

#include <interfaces/INetworkLayer.h>


class ISocket
{

  public:

  /** Tries to send byte sequence. */
  _V bool   send(const Byte *bytes, uint len) = 0;

  /**
    Reads in received data.
    */
  _V uint   receive(BytePtr &bytes, ulong time=ULONG_MAX) = 0;

  _V void   disconnect() = 0;

  _V bool   isConnected() = 0;

};

typedef ISocket*  ISocketPtr;


class IListener
{

  public:

    /**
      Blocks until someone tries to connect.
      */
    _V ISocket* get(ulong time=ULONG_MAX) = 0;

};

typedef IListener*  IListenerPtr;


class ITransportLayer
{
  public:

    _T INetworkLayer::Address  Address;
    _T ushort                 Port;

    _V IListener* createListener(Port port) = 0;

    /** Tries to connect. If fails returns NULL. */
    _V ISocket*   connect(Address address, Port port) = 0;

};

typedef std::shared_ptr<ITransportLayer>  ITransportLayerPtr;

#endif // ITRANSPORTLAYER_H
