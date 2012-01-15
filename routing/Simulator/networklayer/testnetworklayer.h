#ifndef TESTNETWORKLAYER_H
#define TESTNETWORKLAYER_H

#include <QtTest/QTest>
#include <networklayer/networklayer.h>
#include <router/router.h>


class TestNetworkLayer : public QObject
{

  Q_OBJECT

private slots:

  _M void       testInit();
  _M void       testLineTopology();
  _M void       testDifficultTopology();

};

#endif // TESTNETWORKLAYER_H
