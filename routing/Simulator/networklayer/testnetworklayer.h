#ifndef TESTNETWORKLAYER_H
#define TESTNETWORKLAYER_H

#include <QtTest/QTest>
#include <networklayer/networklayer.h>


class TestNetworkLayer : public QObject
{

  Q_OBJECT

private slots:

  _M void       testInit();
  _M void       testLineTopology();

};

#endif // TESTNETWORKLAYER_H
