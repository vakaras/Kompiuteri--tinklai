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
      All information in medium for this client is lost.
      */
    _V void       reconnect() = 0;
};

#endif // IPHYSICALCONNECTION_H
