#ifndef TESTTRANSPORTLAYER_H
#define TESTTRANSPORTLAYER_H

#include <QtTest/QTest>
#include <QObject>
#include <transportlayer/transportlayer.h>


class TestTransportLayer : public QObject
{

  Q_OBJECT

private slots:

  _M void       testInit();
  _M void       testBigAmountOfData();
  _M void       testPingPong();
  _M void       testDisconnect();

};

#endif // TESTTRANSPORTLAYER_H
