#include <QtCore/QCoreApplication>
#include <src/unidirectionalcable/testunidirectionalcable.h>

int main(int argc, char *argv[])
{
  TestUnidirectionalCable test1;
  QTest::qExec(&test1, argc, argv);
//QCoreApplication a(argc, argv);
//return a.exec();
}
