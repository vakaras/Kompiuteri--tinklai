#ifndef IPHYSICALCONNECTION_H
#define IPHYSICALCONNECTION_H


#include <types.h>


/** Class for client connection to medium. */
class IPhysicalConnection
{

  public:

    /**
      Writes bit into medium. Returns false if collision was detected.
      */
    _V bool       write(Bit bit) = 0;

    /**
      Reads bit from medium. If there is nobody writing, than returns
      after time miliseconds.
      */
    _V Bit        read(ulong time=ULONG_MAX, bool *timeOuted=NULL) = 0;

    /**
      All information in medium for this client will be lost.
      */
    _V void       reconnect() = 0;

    /**
      Blocks client thread for a given number of connection cycles.
      */
    _V void       wait(ulong number=1) = 0;

    /**
      Returns true, if number for a number of “cycles” nobody were writing
      to medium.
      */
    _V bool       isFree(uint cycles=1) = 0;
};

#endif // IPHYSICALCONNECTION_H
