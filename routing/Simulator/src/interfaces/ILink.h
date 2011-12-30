#ifndef ILINK_H
#define ILINK_H

#include <src/types.h>


class ILink
{

  public:

    /** Sends len bytes.
      @returns true if successfully send.
      */
    _V bool write(const Byte *bytes, uint len) = 0;

    /** Receives no more than maxlen bytes.
      @param time if there is no data, returns after;
      @returns amount of received bytes
      */
    _V int  read(Byte *bytes, uint maxlen, ulong time=ULONG_MAX) = 0;

    /** Resets to initial state. */
    _V void reset() = 0;
};

#endif // ILINK_H
