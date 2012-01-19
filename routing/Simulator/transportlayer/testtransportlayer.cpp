#include "testtransportlayer.h"
#include <cable/cable.h>
#include <macsublayer/macsublayer.h>
#include <llcsublayer/llcsublayer.h>
#include <networklayer/networklayer.h>
#include <utils/executor.h>

void TestTransportLayer::testInit()
{
  Cable cable(0.0, 0);
  MACSublayer mac1(10, cable.createConnectionPoint());
  MACSublayer mac2(11, cable.createConnectionPoint());
  ILLCSublayerPtr llc1(new LLCSublayer(&mac1));
  ILLCSublayerPtr llc2(new LLCSublayer(&mac2));

  NetworkLayer ntlayer1(1);
  NetworkLayer ntlayer2(2);

  ntlayer1.append(llc1);
  ntlayer2.append(llc2);

  qDebug() << "Creating Transport layer.";
  TransportLayer tlayer1(&ntlayer1);
  TransportLayer tlayer2(&ntlayer2);
  qDebug() << "Transport layer created.";
  auto listen = [&]()
  {
    IListenerPtr listener = tlayer2.createListener(20);
    qDebug() << "Listener created.";
    ISocketPtr socket = listener->get();
    qDebug() << "Incomming connection." << socket;
    QCOMPARE(socket->isConnected(), true);
  };
  Executor executor(listen);
  executor.start();
  QTest::qWait(5000);

  qDebug() << "Trying to connect.";
  ISocketPtr socket = tlayer1.connect(2, 20);
  QVERIFY(socket);
  QCOMPARE(socket->isConnected(), true);
  qDebug() << "Connected.";
  executor.wait();
}
