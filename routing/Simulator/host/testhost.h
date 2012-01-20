#ifndef TESTHOST_H
#define TESTHOST_H

#include <QtTest/QTest>
#include <host/host.h>


class TestHost : public QObject
{

  Q_OBJECT

private slots:

  _M void       testInit();

};

#endif // TESTHOST_H
