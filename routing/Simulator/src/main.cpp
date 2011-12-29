#include <QtCore/QCoreApplication>
#include <src/unidirectionalcable/testunidirectionalcable.h>
#include <src/datalink/testdatalink.h>

int main(int argc, char *argv[])
{
  TestUnidirectionalCable test1;
  QTest::qExec(&test1, argc, argv);

  TestDataLink test2;
  QTest::qExec(&test2, argc, argv);
//QCoreApplication a(argc, argv);
//return a.exec();
}
