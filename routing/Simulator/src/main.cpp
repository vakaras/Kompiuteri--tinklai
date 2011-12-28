#include <QtCore/QCoreApplication>
#include <src/unidirectionalcable/unidirectionalcable.h>

int main(int argc, char *argv[])
{
  UnidirectionalCable cable;
  QCoreApplication a(argc, argv);

  return a.exec();
}
