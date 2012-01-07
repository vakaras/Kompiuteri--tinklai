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

#include <utils/sharedarraydeleter.h>

typedef unsigned char             Byte;
typedef bool                      Bit;
typedef qint64                    MSec;

typedef std::shared_ptr<Byte>     BytePtr;
typedef QList<Bit>                BitList;
typedef QList<Byte>               ByteList;


#endif // TYPES_H
