#ifndef TESTCABLE_H
#define TESTCABLE_H

#include <QtTest/QTest>
#include <cable/cable.h>

class TestCable : public QObject
{

  Q_OBJECT

private slots:

  _M void       testOneWriter();
  _M void       testCollision();

};

#endif // TESTCABLE_H
