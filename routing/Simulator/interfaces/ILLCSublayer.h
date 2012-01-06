#ifndef ILLCSUBLAYER_H
#define ILLCSUBLAYER_H


#include <interfaces/IMACSublayer.h>


class ILLCSublayer
{

  public:

    /**
      Tries to send byte sequence to given address.
      */
    _V bool     send(IMACSublayer::Address, Byte *bytes, uint len) = 0;

    /**
      Broadcasts byte sequence to all connected to same medium.
      */
    _V bool     broadcast(Byte *bytes, uint len) = 0;

    /**
      Reads data portion. If there is no data blocks for maximum
      time miliseconds.
      @param address – addresss from which data arrived;
      @returns a number of bytes received;
      */
    _V uint     receive(IMACSublayer::Address &address,
                        BytePtr &bytes, ulong time=ULONG_MAX) = 0;

    /**
      All information currently in progress will be lost.
      */
    _V void     reconnect() = 0;

};


#endif // ILLCSUBLAYER_H
