#include "testtransportlayer.h"
#include <cable/cable.h>
#include <macsublayer/macsublayer.h>
#include <llcsublayer/llcsublayer.h>
#include <networklayer/networklayer.h>
#include <utils/executor.h>
#include <router/router.h>

#define N 1000u
#define M 20u

void TestTransportLayer::testInit()
{
  Cable cable(1.0/(8*1000), 0);
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

void TestTransportLayer::testBigAmountOfData()
{
  // A -1- R -2- B
  Cable cable1(1.0/(8*2000), 0);
  Cable cable2(1.0/(8*2000), 0);
  MACSublayer macA(10, cable1.createConnectionPoint());
  MACSublayer macB(11, cable2.createConnectionPoint());
  ILLCSublayerPtr llcA(new LLCSublayer(&macA));
  ILLCSublayerPtr llcB(new LLCSublayer(&macB));

  Router router(3);

  NetworkLayer ntlayerA(1);
  NetworkLayer ntlayerB(2);
  ntlayerA.append(llcA);
  ntlayerB.append(llcB);
  router.connect(cable1, 12);
  router.connect(cable2, 13);

  qDebug() << "Creating Transport layer.";
  TransportLayer tlayerA(&ntlayerA);
  TransportLayer tlayerB(&ntlayerB);
  qDebug() << "Transport layer created.";

  QTest::qWait(5000);

  QCOMPARE(tlayerA.connect(2, 20), (ISocketPtr) NULL);
  QCOMPARE(tlayerA.connect(3, 20), (ISocketPtr) NULL);

  Byte data[N];
  for (uint i = 0; i < N; i++)
  {
    data[i] = i & 15;
  }

  ISocketPtr socket1;
  ISocketPtr socket2;
  ISocketPtr socketClient1;
  ISocketPtr socketClient2;

  auto send1 = [&]()
  {
    QCOMPARE(socket1->isConnected(), true);
    QCOMPARE(socket1->send(data, N), true);
  };

  auto send2 = [&]()
  {
    QCOMPARE(socket2->isConnected(), true);
    QCOMPARE(socket2->send(data, N), true);
  };

  auto receive1 = [&]()
  {
    QCOMPARE(socketClient1->isConnected(), true);
    BytePtr bytes;
    uint len = 0;
    Byte buffer[N];
    for (uint i = 0; i < N; i += len)
    {
      len = socketClient1->receive(bytes);
      for (uint j = 0; j < len; j++)
      {
        buffer[i + j] = bytes.get()[j];
      }
    }
    for (uint i = 0; i < N; i++)
    {
      QCOMPARE(buffer[i], data[i]);
    }
  };

  auto receive2 = [&]()
  {
    QCOMPARE(socketClient2->isConnected(), true);
    BytePtr bytes;
    uint len = 0;
    Byte buffer[N];
    for (uint i = 0; i < N; i += len)
    {
      len = socketClient2->receive(bytes);
      for (uint j = 0; j < len; j++)
      {
        buffer[i + j] = bytes.get()[j];
      }
    }
    for (uint i = 0; i < N; i++)
    {
      QCOMPARE(buffer[i], data[i]);
    }
  };

  auto server = [&]()
  {
    IListenerPtr listener = tlayerB.createListener(20);
    Executor executor1(send1);
    Executor executor2(send2);
    socket1 = listener->get();
    executor1.start();
    socket2 = listener->get();
    executor2.start();
    executor1.wait();
    executor2.wait();
    qDebug() << "Server end.";
  };
  Executor executor(server);
  executor.start();

  Executor client1(receive1);
  Executor client2(receive2);
  socketClient1 = tlayerA.connect(2, 20);
  QVERIFY(socketClient1);
  client1.start();

  socketClient2 = tlayerA.connect(2, 20);
  QVERIFY(socketClient2);
  client2.start();
  qDebug() << "Connected.";

  executor.wait();
  client1.wait();
  client2.wait();
}

void TestTransportLayer::testPingPong()
{
  // A -1- R -2- B
  Cable cable1(0.0, 0);
  Cable cable2(0.0, 0);
  MACSublayer macA(10, cable1.createConnectionPoint());
  MACSublayer macB(11, cable2.createConnectionPoint());
  ILLCSublayerPtr llcA(new LLCSublayer(&macA));
  ILLCSublayerPtr llcB(new LLCSublayer(&macB));

  Router router(3);

  NetworkLayer ntlayerA(1);
  NetworkLayer ntlayerB(2);
  ntlayerA.append(llcA);
  ntlayerB.append(llcB);
  router.connect(cable1, 12);
  router.connect(cable2, 13);

  qDebug() << "Creating Transport layer.";
  TransportLayer tlayerA(&ntlayerA);
  TransportLayer tlayerB(&ntlayerB);
  qDebug() << "Transport layer created.";

  auto listen = [&]()
  {
    IListenerPtr listener = tlayerA.createListener(20);
    qDebug() << "Listener created.";
    ISocketPtr socket = listener->get();
    qDebug() << "Incomming connection." << socket;
    QCOMPARE(socket->isConnected(), true);
    Byte data[] = {0};
    BytePtr bytes;
    uint len;
    for (uint i = 0; i < M; i++)
    {
      data[0] = i;
      QCOMPARE(socket->send(data, 1), true);

      len = socket->receive(bytes);
      QCOMPARE(len, 1u);
      qDebug() << bytes.get()[0] << ((Byte) i);
      QCOMPARE(bytes.get()[0], ((Byte) (i + 10)));
    }
  };
  Executor executor(listen);
  executor.start();
  QTest::qWait(5000);

  qDebug() << "Trying to connect.";
  ISocketPtr socket = tlayerB.connect(1, 20);
  QVERIFY(socket);
  QCOMPARE(socket->isConnected(), true);
  qDebug() << "Connected.";

  BytePtr bytes;
  uint len;
  Byte data[] = {0};
  for (uint i = 0; i < M; i++)
  {
    len = socket->receive(bytes);
    QCOMPARE(len, 1u);
    qDebug() << bytes.get()[0] << ((Byte) i);
    QCOMPARE(bytes.get()[0], ((Byte) i));

    data[0] = i + 10;
    QCOMPARE(socket->send(data, 1), true);
  }

  executor.wait();
}

void TestTransportLayer::testDisconnect()
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

  Byte data[] = {5, 4, 3, 2, 1, 0, 1, 2, 3, 4, 5};
  auto listen = [&]()
  {
    IListenerPtr listener = tlayer2.createListener(20);
    qDebug() << "Listener created.";
    ISocketPtr socket = listener->get();
    qDebug() << "Incomming connection." << socket;
    QCOMPARE(socket->isConnected(), true);
    QCOMPARE(socket->send(data, 11), true);
    qDebug() << "     Server sent data.";
    QCOMPARE(tlayer2.remove(socket), true);
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
  QCOMPARE(len, 11u);
  pointer = bytes.get();
  for (auto byte : data)
  {
    QCOMPARE(*(pointer++), byte);
  }
  QTest::qWait(2000);
  qDebug() << "Is connected:" << socket->isConnected();
  QCOMPARE(socket->isConnected(), false);
  QCOMPARE(tlayer1.remove(socket), true);

  executor.wait();
}
