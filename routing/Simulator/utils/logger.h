#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

//#define LOG_PHYSICAL
//#define LOG_MAC
//#define LOG_NETWORK
//#define LOG_TRANSPORT

#ifdef LOG_PHYSICAL
#define PLOG(x) qDebug() << this << x;
#else
#define PLOG(x)
#endif

#ifdef LOG_MAC
#define MLOG(x) qDebug() << this << x;
#else
#define MLOG(x)
#endif

#ifdef LOG_NETWORK
#define NLOG(x) qDebug() << this << x;
#else
#define NLOG(x)
#endif

#ifdef LOG_TRANSPORT
#define TLOG(x) qDebug() << this << x;
#else
#define TLOG(x)
#endif

#endif // LOGGER_H
