#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

//#define LOG_PHYSICAL
//#define LOG_MAC

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

#endif // LOGGER_H
