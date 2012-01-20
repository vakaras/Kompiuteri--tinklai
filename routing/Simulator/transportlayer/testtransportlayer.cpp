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
  Byte data1[] = {2, 7, 1, 8, 2, 8, 1, 8, 2, 8, 4, 5, 9, 0, 4, 5};
  Byte data2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  Byte data3[] = {0, 9, 8, 7, 6, 5, 4, 3, 2, 1};
  Byte data4[] = {0, 1, 2, 3, 4, 5, 5, 4, 3, 2, 1, 0};
  Byte data5[] = {5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5};
  auto listen = [&]()
  {
    IListenerPtr listener = tlayer2.createListener(20);
    qDebug() << "Listener created.";
    ISocketPtr socket = listener->get();
    qDebug() << "Incomming connection." << socket;
    QCOMPARE(socket->isConnected(), true);
    QCOMPARE(socket->send(data1, 16), true);
    QCOMPARE(socket->send(data2, 10), true);
    QCOMPARE(socket->send(data3, 10), true);

    qDebug() << "     Server sent data.";
    BytePtr bytes;
    uint len;
    Byte *pointer;

    len = socket->receive(bytes);
    pointer = bytes.get();
    for (uint i = 0; i < len; i++)
    {
      qDebug() << "Server received Byte" << i << *(pointer + i);
    }
    QCOMPARE(len, 12u);
    for (auto byte : data4)
    {
      QCOMPARE(*(pointer++), byte);
    }

    len = socket->receive(bytes);
    QCOMPARE(len, 11u);
    pointer = bytes.get();
    for (auto byte : data5)
    {
      QCOMPARE(*(pointer++), byte);
    }
    qDebug() << "END------------------";
  };
  Executor executor(listen);
  executor.start();
  QTest::qWait(5000);

  qDebug() << "Trying to connect.";
  ISocketPtr socket = tlayer1.connect(2, 20);
  QVERIFY(socket);
  QCOMPARE(socket->isConnected(), true);
  qDebug() << "Connected.";

  BytePtr bytes;
  uint len;
  Byte *pointer;

  len = socket->receive(bytes);
  QCOMPARE(len, 16u);
  pointer = bytes.get();
  for (auto byte : data1)
  {
    QCOMPARE(*(pointer++), byte);
  }

  len = socket->receive(bytes);
  QCOMPARE(len, 10u);
  pointer = bytes.get();
  for (auto byte : data2)
  {
    QCOMPARE(*(pointer++), byte);
  }

  len = socket->receive(bytes);
  QCOMPARE(len, 10u);
  pointer = bytes.get();
  for (auto byte : data3)
  {
    qDebug() << *pointer << byte;
    QCOMPARE(*(pointer++), byte);
  }

  qDebug() << "                 Client is sending data.";
  QCOMPARE(socket->send(data4, 12), true);
  QCOMPARE(socket->send(data5, 11), true);

  qDebug() << "----------------------------------------Read data.";
  executor.wait();
  qDebug() << "Kill.";
}
