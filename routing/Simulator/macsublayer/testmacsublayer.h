#ifndef TESTMACSUBLAYER_H
#define TESTMACSUBLAYER_H


#include <QtTest/QTest>
#include <macsublayer/macsublayer.h>


class TestMACSublayer : public QObject
{

  Q_OBJECT

private slots:

  _M void       testInit();
  _M void       testDeletingCableClient();
  _M void       testCollision();

};

#endif // TESTMACSUBLAYER_H
