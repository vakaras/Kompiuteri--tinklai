#ifndef TYPES_H
#define TYPES_H

#include <QDebug>
#include <QList>
#include <memory>

#define _C static const
#define _E enum
#define _F friend
#define _M
#define _S static
#define _T typedef
#define _V virtual
#define _X mutable
#define _Y const

#define Vacuum

typedef unsigned char             Byte;
typedef bool                      Bit;

typedef std::shared_ptr<Byte>     BytePtr;
typedef QList<Bit>                BitList;
typedef QList<Byte>               ByteList;


#endif // TYPES_H
