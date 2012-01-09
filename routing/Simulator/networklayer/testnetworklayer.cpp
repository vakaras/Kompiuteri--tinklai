#include "testnetworklayer.h"
#include <cable/cable.h>
#include <macsublayer/macsublayer.h>
#include <llcsublayer/llcsublayer.h>

void TestNetworkLayer::testInit()
{
  Cable cable(0.0, 0);
  MACSublayer mac1(10, cable.createConnectionPoint());
  MACSublayer mac2(11, cable.createConnectionPoint());
  MACSublayer mac3(12, cable.createConnectionPoint());
  ILLCSublayerPtr llc1(new LLCSublayer(&mac1));
  ILLCSublayerPtr llc2(new LLCSublayer(&mac2));
  ILLCSublayerPtr llc3(new LLCSublayer(&mac3));

  NetworkLayer ntlayer1(1);
  NetworkLayer ntlayer2(2);
  NetworkLayer ntlayer3(3);

  qDebug() << "W1";
  ntlayer1.append(llc1);
  ntlayer2.append(llc2);
  ntlayer3.append(llc3);
  qDebug() << "W2";

  QTest::qWait(1000);
}
