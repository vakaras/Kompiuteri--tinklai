#ifndef TESTDATALINK_H
#define TESTDATALINK_H

#include <QtTest/QTest>
#include <src/datalink/datalink.h>

class TestDataLink : public QObject
{

  Q_OBJECT

private:

private slots:

  _M void   testInit();
  _M void   testFrameHeader();
  _M void   testEncodingFrame();
  _M void   testDecodingFrame();

};

#endif // TESTDATALINK_H
