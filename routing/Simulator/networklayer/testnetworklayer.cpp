#include "testnetworklayer.h"

void TestNetworkLayer::testInit()
{
  NetworkLayer ntlayer1(1);
  NetworkLayer ntlayer2(2);

  QTest::qWait(1000);
}
