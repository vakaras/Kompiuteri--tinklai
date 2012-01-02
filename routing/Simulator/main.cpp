#include <QtCore/QCoreApplication>
#include <cable/testcable.h>
#include <macsublayer/testmacsublayer.h>


int main(int argc, char *argv[])
{
  TestCable test1;
  QTest::qExec(&test1, argc, argv);

  TestMACSublayer test2;
  QTest::qExec(&test2, argc, argv);

  return 0;
}
