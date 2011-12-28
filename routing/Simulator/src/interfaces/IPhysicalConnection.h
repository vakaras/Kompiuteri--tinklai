#ifndef IPHYSICALCONNECTION_H
#define IPHYSICALCONNECTION_H

#include <src/types.h>


class IPhysicalConnection
{

  public:

    _V void write(Byte byte) = 0;
    _V Byte read() = 0;                 // Blocks if there is no data
                                        // available.
};

#endif // IPHYSICALCONNECTION_H
