#include <QtCore/QCoreApplication>
#include <cable/testcable.h>
#include <macsublayer/testmacsublayer.h>
#include <networklayer/testnetworklayer.h>


int main(int argc, char *argv[])
{
  TestCable test1;
  QTest::qExec(&test1, argc, argv);

  TestMACSublayer test2;
  QTest::qExec(&test2, argc, argv);

  TestNetworkLayer test3;
  QTest::qExec(&test3, argc, argv);

  return 0;
}
