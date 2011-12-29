#ifndef IPHYSICALCONNECTION_H
#define IPHYSICALCONNECTION_H

#include <src/types.h>


class IPhysicalConnection
{

  public:

    _V void write(Bit bit) = 0;
    _V Bit  read(ulong time=ULONG_MAX, bool *timeOuted=NULL) = 0;
                                        // Blocks if there is no data
                                        // available.
    _V void reset() = 0;                // Reinitializes connection.
};

#endif // IPHYSICALCONNECTION_H
