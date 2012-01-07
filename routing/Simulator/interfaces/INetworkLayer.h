#ifndef INETWORKLAYER_H
#define INETWORKLAYER_H


#include <types.h>


class INetworLayer
{

  public:

    _T uint   Address;

    /**
      Tries to send byte sequence to given address.
      */
    _V bool   send(Address address, Byte *bytes, uint len) = 0;

    /**
      Reads data portion. If there is no data blocks for maximum
      time miliseconds.
      @param address – addresss from which data arrived;
      @returns a number of bytes received;
      */
    _V uint   receive(Address &address, BytePtr &bytes,
                      ulong time=ULONG_MAX) = 0;

    /**
      All information currently in progress will be lost.
      */
    _V void   restart() = 0;
};

#endif // INETWORKLAYER_H
