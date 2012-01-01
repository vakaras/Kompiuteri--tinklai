#include <QtCore/QCoreApplication>
#include <cable/testcable.h>

int main(int argc, char *argv[])
{
  TestCable test1;
  QTest::qExec(&test1, argc, argv);

  return 0;
}
