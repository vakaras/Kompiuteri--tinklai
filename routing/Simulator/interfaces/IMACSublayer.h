#ifndef IMACSUBLAYER_H
#define IMACSUBLAYER_H


#include <types.h>

extern uint MAC_BROADCAST_ADDRESS;


class IMACSublayer
{

  public:

    _T uint     Address;

    /**
      Tries to send byte sequence to given address.
      */
    _V bool     write(Address address, Byte *bytes, uint len) = 0;

    /**
      Reads data portion. If there is no data blocks for maximum
      time miliseconds.
      @param address – addresss from which data arrived;
      @returns a number of bytes received;
      */
    _V uint     read(Address &address,
                     BytePtr &bytes, ulong time=ULONG_MAX) = 0;

    /**
      All information currently in progress will be lost.
      */
    _V void     reconnect() = 0;

    /**
      @returns MAC address.
      */
    _V Address  getAddress() = 0;

};

typedef std::shared_ptr<IMACSublayer>   IMACSublayerPtr;

#endif // IMACSUBLAYER_H
