#include "testcable.h"
#include <utils/executor.h>


void TestCable::testOneWriter()
{
  Cable cable(0.0, 0);
  IPhysicalConnectionPtr cp1 = cable.createConnectionPoint();
  IPhysicalConnectionPtr cp2 = cable.createConnectionPoint();
  IPhysicalConnectionPtr cp3 = cable.createConnectionPoint();

  auto write1 = [&]()
  {
    QCOMPARE(cp1->write(1), true);
  };
  Executor writer1(write1);
  writer1.start();

  QCOMPARE(cp1->read(), 1);
  QCOMPARE(cp2->read(), 1);
  QCOMPARE(cp3->read(), 1);

  cp1->wait(3);
  writer1.wait();
}

void TestCable::testCollision()
{
  Cable cable(0.0, 500);
  IPhysicalConnectionPtr cp1 = cable.createConnectionPoint();
  IPhysicalConnectionPtr cp2 = cable.createConnectionPoint();
  IPhysicalConnectionPtr cp3 = cable.createConnectionPoint();

  Executor writer1([&]() { QCOMPARE(cp1->write(0), false); });
  Executor writer2([&]() { QCOMPARE(cp2->write(1), false); });
  Executor writer3([&]() { QCOMPARE(cp3->write(0), false); });
  writer1.start();
  writer2.start();
  writer3.start();

  QCOMPARE(cp1->read(), 1);
  QCOMPARE(cp2->read(), 1);
  QCOMPARE(cp3->read(), 1);

  writer1.wait();
  writer2.wait();
  writer3.wait();
}
