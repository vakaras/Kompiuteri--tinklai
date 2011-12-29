#ifndef ILINK_H
#define ILINK_H

#include <src/types.h>


class ILink
{

  public:

    /** Sends len bytes.
      @returns true if successfully send.
      */
    _V bool write(const Byte *bytes, int len) = 0;

    /** Receives no more than maxlen bytes.
      @param block if true, thread blocks while there is no data.
      @returns amount of received bytes
      */
    _V int  read(Byte *bytes, int maxlen, bool block=true) = 0;

    /** Resets to initial state. */
    _V void reset();
};

#endif // ILINK_H
