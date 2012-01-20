#include <QtCore/QCoreApplication>
#include <cable/testcable.h>
#include <macsublayer/testmacsublayer.h>
#include <networklayer/testnetworklayer.h>
#include <transportlayer/testtransportlayer.h>

// MAC sublayer
uint  MAC_BROADCAST_ADDRESS = 0xFFFF;
uint  MAC_FRAME_DATA_SIZE_MAX = 512;
uint  MAC_FRAME_SIZE_MIN = 64;
// Network layer
uint  ROUTES_UPDATE_PERIOD = 10000;
int   MAX_READ_BUFFER_SIZE = 20;
uint  MAX_HOPS = 20;
uint  NEIGHBOUR_INFO_LIFETIME = 20000;
// Transport layer
uint MAX_SEGMENT_SIZE = (1u << 8);
uint MAX_BUFFER_SIZE = (1u << 14);
uint MAX_SEGMENT_AMOUNT = (MAX_BUFFER_SIZE/MAX_SEGMENT_SIZE);

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  TestCable test1;
  QTest::qExec(&test1, argc, argv);

  TestMACSublayer test2;
  QTest::qExec(&test2, argc, argv);

  TestNetworkLayer test3;
  QTest::qExec(&test3, argc, argv);

  TestTransportLayer test4;
  QTest::qExec(&test4, argc, argv);

  return 0;
}
