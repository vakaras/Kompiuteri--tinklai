#include "testnetworklayer.h"
#include <cable/cable.h>
#include <macsublayer/macsublayer.h>
#include <llcsublayer/llcsublayer.h>
#include <QDateTime>


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

  qDebug() << "NT1:" << &ntlayer1;
  qDebug() << "NT2:" << &ntlayer2;
  qDebug() << "NT3:" << &ntlayer3;

  QCOMPARE(ntlayer1.m_address, ntlayer1.m_routingTable.m_address);
  QCOMPARE(ntlayer2.m_address, ntlayer2.m_routingTable.m_address);
  QCOMPARE(ntlayer3.m_address, ntlayer3.m_routingTable.m_address);

  qDebug() << "W1";
  ntlayer1.append(llc1);
  ntlayer2.append(llc2);
  ntlayer3.append(llc3);
  qDebug() << "W2";

  QTest::qWait(8000);
  qDebug() << "W3";
  QCOMPARE(ntlayer1.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer1.m_neighbourMap[mac2.getAddress()].m_distance < 1000);
  QVERIFY(ntlayer1.m_neighbourMap[mac3.getAddress()].m_distance < 1000);
  QCOMPARE(ntlayer2.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer2.m_neighbourMap[mac1.getAddress()].m_distance < 1000);
  QVERIFY(ntlayer2.m_neighbourMap[mac3.getAddress()].m_distance < 1000);
  QCOMPARE(ntlayer3.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer3.m_neighbourMap[mac1.getAddress()].m_distance < 1000);
  QVERIFY(ntlayer3.m_neighbourMap[mac2.getAddress()].m_distance < 1000);

  QCOMPARE(ntlayer1.m_routingTable.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer1.m_routingTable.m_neighbourMap.contains(ntlayer2.m_address));
  QVERIFY(ntlayer1.m_routingTable.m_neighbourMap.contains(ntlayer3.m_address));
  QCOMPARE(ntlayer2.m_routingTable.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer2.m_routingTable.m_neighbourMap.contains(ntlayer1.m_address));
  QVERIFY(ntlayer2.m_routingTable.m_neighbourMap.contains(ntlayer3.m_address));
  QCOMPARE(ntlayer3.m_routingTable.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer3.m_routingTable.m_neighbourMap.contains(ntlayer1.m_address));
  QVERIFY(ntlayer3.m_routingTable.m_neighbourMap.contains(ntlayer2.m_address));

  //QCOMPARE(ntlayer1.m_routingTable.m_routerInfoMap.size(), 2);
  if (ntlayer1.m_routingTable.m_routerInfoMap.size() == 2)
  {
    QVERIFY(ntlayer1.m_routingTable.m_routerInfoMap.contains(ntlayer2.m_address));
    QVERIFY(ntlayer1.m_routingTable.m_routerInfoMap.contains(ntlayer3.m_address));
  }
  else
  {
    for (auto it : ntlayer1.m_routingTable.m_routerInfoMap)
    {
      qDebug() << "IP:" << it.m_ipAddress << it.m_sequenceNumber;
    }
    QFAIL("Something went wrong.");
  }
  //QCOMPARE(ntlayer2.m_routingTable.m_routerInfoMap.size(), 2);
  if (ntlayer2.m_routingTable.m_routerInfoMap.size() == 2)
  {
    QVERIFY(ntlayer2.m_routingTable.m_routerInfoMap.contains(ntlayer1.m_address));
    QVERIFY(ntlayer2.m_routingTable.m_routerInfoMap.contains(ntlayer3.m_address));
  }
  else
  {
    for (auto it : ntlayer2.m_routingTable.m_routerInfoMap)
    {
      qDebug() << "IP:" << it.m_ipAddress << it.m_sequenceNumber;
    }
    QFAIL("Something went wrong.");
  }
  //QCOMPARE(ntlayer3.m_routingTable.m_routerInfoMap.size(), 2);
  if (ntlayer3.m_routingTable.m_routerInfoMap.size() == 2)
  {
    QVERIFY(ntlayer3.m_routingTable.m_routerInfoMap.contains(ntlayer1.m_address));
    QVERIFY(ntlayer3.m_routingTable.m_routerInfoMap.contains(ntlayer2.m_address));
  }
  else
  {
    for (auto it : ntlayer3.m_routingTable.m_routerInfoMap)
    {
      qDebug() << "IP:" << it.m_ipAddress << it.m_sequenceNumber;
    }
    QFAIL("Something went wrong.");
  }

  auto r1_2 = ntlayer1.m_routingTable.m_routerInfoMap[ntlayer2.m_address];
  QCOMPARE(r1_2.m_knowsInfoSet.size(), 2);
  QCOMPARE(r1_2.m_through, ntlayer2.m_address);
  QCOMPARE(r1_2.m_distance, ntlayer1.m_neighbourMap[mac2.getAddress()].m_distance);

  auto r1_3 = ntlayer1.m_routingTable.m_routerInfoMap[ntlayer3.m_address];
  QCOMPARE(r1_3.m_knowsInfoSet.size(), 2);
  QCOMPARE(r1_3.m_through, ntlayer3.m_address);
  QCOMPARE(r1_3.m_distance, ntlayer1.m_neighbourMap[mac3.getAddress()].m_distance);

  auto r2_1 = ntlayer2.m_routingTable.m_routerInfoMap[ntlayer1.m_address];
  QCOMPARE(r2_1.m_knowsInfoSet.size(), 2);
  QCOMPARE(r2_1.m_through, ntlayer1.m_address);
  QCOMPARE(r2_1.m_distance, ntlayer2.m_neighbourMap[mac1.getAddress()].m_distance);

  auto r2_3 = ntlayer2.m_routingTable.m_routerInfoMap[ntlayer3.m_address];
  QCOMPARE(r2_3.m_knowsInfoSet.size(), 2);
  QCOMPARE(r2_3.m_through, ntlayer3.m_address);
  QCOMPARE(r2_3.m_distance, ntlayer2.m_neighbourMap[mac3.getAddress()].m_distance);

  auto r3_1 = ntlayer3.m_routingTable.m_routerInfoMap[ntlayer1.m_address];
  QCOMPARE(r3_1.m_knowsInfoSet.size(), 2);
  QCOMPARE(r3_1.m_through, ntlayer1.m_address);
  QCOMPARE(r3_1.m_distance, ntlayer3.m_neighbourMap[mac1.getAddress()].m_distance);

  auto r3_2 = ntlayer3.m_routingTable.m_routerInfoMap[ntlayer2.m_address];
  QCOMPARE(r3_2.m_knowsInfoSet.size(), 2);
  QCOMPARE(r3_2.m_through, ntlayer2.m_address);
  QCOMPARE(r3_2.m_distance, ntlayer3.m_neighbourMap[mac2.getAddress()].m_distance);
}

void TestNetworkLayer::testLineTopology()
{
  // Model:
  // A -1- B -2- C
  Cable cable1(0.0, 0);
  Cable cable2(0.0, 0);

  MACSublayer macA1(10, cable1.createConnectionPoint());
  MACSublayer macB1(11, cable1.createConnectionPoint());
  MACSublayer macB2(12, cable2.createConnectionPoint());
  MACSublayer macC2(13, cable2.createConnectionPoint());
  ILLCSublayerPtr llcA1(new LLCSublayer(&macA1));
  ILLCSublayerPtr llcB1(new LLCSublayer(&macB1));
  ILLCSublayerPtr llcB2(new LLCSublayer(&macB2));
  ILLCSublayerPtr llcC2(new LLCSublayer(&macC2));

  NetworkLayer ntlayerA(1);
  NetworkLayer ntlayerB(2);
  NetworkLayer ntlayerC(3);

  qDebug() << "NTA:" << &ntlayerA;
  qDebug() << "NTB:" << &ntlayerB;
  qDebug() << "NTC:" << &ntlayerC;

  qDebug() << "W1";
  ntlayerA.append(llcA1);
  ntlayerB.append(llcB1);
  ntlayerB.append(llcB2);
  ntlayerC.append(llcC2);
  qDebug() << "W2";

  QTest::qWait(8000);
  qDebug() << "W3";
  QCOMPARE(ntlayerA.m_neighbourMap.size(), 1);
  QVERIFY(ntlayerA.m_neighbourMap[macB1.getAddress()].m_distance < 1000);
  QCOMPARE(ntlayerB.m_neighbourMap.size(), 2);
  QVERIFY(ntlayerB.m_neighbourMap[macA1.getAddress()].m_distance < 1000);
  QVERIFY(ntlayerB.m_neighbourMap[macC2.getAddress()].m_distance < 1000);
  QCOMPARE(ntlayerC.m_neighbourMap.size(), 1);
  QVERIFY(ntlayerC.m_neighbourMap[macB2.getAddress()].m_distance < 1000);

  QCOMPARE(ntlayerA.m_routingTable.m_neighbourMap.size(), 1);
  QVERIFY(ntlayerA.m_routingTable.m_neighbourMap.contains(ntlayerB.m_address));
  QCOMPARE(ntlayerB.m_routingTable.m_neighbourMap.size(), 2);
  QVERIFY(ntlayerB.m_routingTable.m_neighbourMap.contains(ntlayerA.m_address));
  QVERIFY(ntlayerB.m_routingTable.m_neighbourMap.contains(ntlayerC.m_address));
  QCOMPARE(ntlayerC.m_routingTable.m_neighbourMap.size(), 1);
  QVERIFY(ntlayerC.m_routingTable.m_neighbourMap.contains(ntlayerB.m_address));

  QCOMPARE(ntlayerA.m_routingTable.m_routerInfoMap.size(), 2);
  QVERIFY(ntlayerA.m_routingTable.m_routerInfoMap.contains(ntlayerB.m_address));
  QVERIFY(ntlayerA.m_routingTable.m_routerInfoMap.contains(ntlayerC.m_address));
  QCOMPARE(ntlayerB.m_routingTable.m_routerInfoMap.size(), 2);
  QVERIFY(ntlayerB.m_routingTable.m_routerInfoMap.contains(ntlayerA.m_address));
  QVERIFY(ntlayerB.m_routingTable.m_routerInfoMap.contains(ntlayerC.m_address));
  QCOMPARE(ntlayerC.m_routingTable.m_routerInfoMap.size(), 2);
  QVERIFY(ntlayerC.m_routingTable.m_routerInfoMap.contains(ntlayerA.m_address));
  QVERIFY(ntlayerC.m_routingTable.m_routerInfoMap.contains(ntlayerB.m_address));

  auto rA_B = ntlayerA.m_routingTable.m_routerInfoMap[ntlayerB.m_address];
  QCOMPARE(rA_B.m_knowsInfoSet.size(), 1);
  QCOMPARE(rA_B.m_through, ntlayerB.m_address);
  QCOMPARE(rA_B.m_distance, ntlayerA.m_neighbourMap[macB1.getAddress()].m_distance);

  auto rA_C = ntlayerA.m_routingTable.m_routerInfoMap[ntlayerC.m_address];
  QCOMPARE(rA_C.m_knowsInfoSet.size(), 1);
  QCOMPARE(rA_C.m_through, ntlayerB.m_address);
  QVERIFY(rA_C.m_distance > ntlayerA.m_neighbourMap[macB1.getAddress()].m_distance);

  auto rB_A = ntlayerB.m_routingTable.m_routerInfoMap[ntlayerA.m_address];
  QCOMPARE(rB_A.m_knowsInfoSet.size(), 2);
  QCOMPARE(rB_A.m_through, ntlayerA.m_address);
  QCOMPARE(rB_A.m_distance, ntlayerB.m_neighbourMap[macA1.getAddress()].m_distance);

  auto rB_C = ntlayerB.m_routingTable.m_routerInfoMap[ntlayerC.m_address];
  QCOMPARE(rB_C.m_knowsInfoSet.size(), 2);
  QCOMPARE(rB_C.m_through, ntlayerC.m_address);
  QCOMPARE(rB_C.m_distance, ntlayerB.m_neighbourMap[macC2.getAddress()].m_distance);

  auto rC_B = ntlayerC.m_routingTable.m_routerInfoMap[ntlayerB.m_address];
  QCOMPARE(rC_B.m_knowsInfoSet.size(), 1);
  QCOMPARE(rC_B.m_through, ntlayerB.m_address);
  QCOMPARE(rC_B.m_distance, ntlayerC.m_neighbourMap[macB2.getAddress()].m_distance);

  auto rC_A = ntlayerC.m_routingTable.m_routerInfoMap[ntlayerA.m_address];
  QCOMPARE(rC_A.m_knowsInfoSet.size(), 1);
  QCOMPARE(rC_A.m_through, ntlayerB.m_address);
  QVERIFY(rC_A.m_distance > ntlayerC.m_neighbourMap[macB2.getAddress()].m_distance);

  qDebug() << "W4";
  Byte data[] = {4, 5, 6, 7, 8, 9};
  QCOMPARE(ntlayerA.send(ntlayerA.m_address, data, 6), false);
  QCOMPARE(ntlayerA.send(4, data, 6), false);
  QCOMPARE(ntlayerA.send(ntlayerC.m_address, data, 6), true);
  INetworkLayer::Address address;
  BytePtr bytes;
  QCOMPARE(ntlayerC.receive(address, bytes), 6u);
  QCOMPARE(address, ntlayerA.m_address);
  Byte *pointer = bytes.get();
  for (auto byte : data)
  {
    QCOMPARE(*(pointer++), byte);
  }

}

void TestNetworkLayer::testDifficultTopology()
{
  /* Model:
   *
   *    1
   *    |          H0
   *    |          |
   *    |          0
   *    |          |
   *    |----------H1-----2-----H2
   *    |          |            |
   *    |          |            |
   *    |          3            4
   *    |          |            |
   *    |          |            |
   *    |          H3-----5-----H4
   *    |
   *    |
   *    |-------------------H5-----6-------H6    
   *    |                   | \           /|    
   *    |                   |  7         / |    
   *    |                   |   \       8  |    
   *    |                   |    \     /   |    
   *    |                   |     \   /    |    
   *    |                   |      \ /     |    
   *    |                   9       \      10   
   *    |                   |      / \     |    
   *    |                   |     /   \    |    
   *    |                   |    /     \   |    
   *    |                   |   /       \  |    
   *    |                   |  /         \ |    
   *    |                   | /           \|    
   *    |-------------------H7-----11------H8    
   */

  Cable cables[12];
  Router *routers[9];
  for (int i = 0; i < 9; i++)
  {
    routers[i] = new Router(100 + i);
  }

  routers[0]->connect(cables[0], 1);

  routers[1]->connect(cables[0], 2);
  routers[1]->connect(cables[1], 3);
  routers[1]->connect(cables[2], 4);
  routers[1]->connect(cables[3], 5);

  routers[2]->connect(cables[2], 6);
  routers[2]->connect(cables[4], 7);

  routers[3]->connect(cables[3], 8);
  routers[3]->connect(cables[5], 9);

  routers[4]->connect(cables[4], 10);
  routers[4]->connect(cables[5], 11);

  QTest::qWait(8000);

  Byte data[] = {4, 5, 6, 7, 8, 9};
  INetworkLayer::Address address;
  BytePtr bytes;
  Byte *pointer;

  QCOMPARE(routers[4]->m_networkLayer.m_neighbourMap.size(), 2);
  QCOMPARE(routers[4]->m_networkLayer.m_routingTable.m_neighbourMap.size(), 2);
  QCOMPARE(routers[4]->m_networkLayer.m_routingTable.m_routerInfoMap.size(), 4);
  QCOMPARE(routers[4]->send(routers[0]->address(), data, 6), true);
  QCOMPARE(routers[0]->receive(address, bytes), 6u);
  QCOMPARE(address, routers[4]->m_address);
  pointer = bytes.get();
  for (auto byte : data)
  {
    QCOMPARE(*(pointer++), byte);
  }

  routers[5]->connect(cables[1], 12);
  routers[5]->connect(cables[6], 13);
  routers[5]->connect(cables[7], 14);
  routers[5]->connect(cables[9], 15);

  routers[6]->connect(cables[6],  16);
  routers[6]->connect(cables[8],  17);
  routers[6]->connect(cables[10], 18);

  routers[7]->connect(cables[1],  18);
  routers[7]->connect(cables[8],  19);
  routers[7]->connect(cables[9],  20);
  routers[7]->connect(cables[11], 21);

  routers[8]->connect(cables[7],  22);
  routers[8]->connect(cables[10], 23);
  routers[8]->connect(cables[11], 24);

  qDebug() << "W4";
  QTest::qWait(20000);
  qDebug() << "W5";

  QCOMPARE(routers[4]->m_networkLayer.m_routingTable.m_routerInfoMap.size(), 8);
  QCOMPARE(routers[4]->send(routers[8]->address(), data, 6), true);
  qDebug() << "W5.1";
  QCOMPARE(routers[8]->receive(address, bytes), 6u);
  qDebug() << "W5.2";
  QCOMPARE(address, routers[4]->m_address);
  pointer = bytes.get();

  qDebug() << "W6";
  delete routers[7];
  routers[7] = new Router(100 + 7);
  routers[5]->disconnect(3);
  QTest::qWait(30000);
  qDebug() << "W7";
  QCOMPARE(routers[5]->m_networkLayer.m_neighbourMap.size(), 2);
  QCOMPARE(routers[4]->m_networkLayer.m_routingTable.m_routerInfoMap.size(), 7);
  QCOMPARE(routers[4]->send(routers[8]->address(), data, 6), true);
  qDebug() << "W7.1";
  QCOMPARE(routers[8]->receive(address, bytes), 6u);
  qDebug() << "W7.2";
  QCOMPARE(address, routers[4]->m_address);
  pointer = bytes.get();
  qDebug() << "W8";

  for (auto router : routers)
    delete router;
}
