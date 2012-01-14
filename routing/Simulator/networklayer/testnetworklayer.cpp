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
  QCOMPARE(ntlayer2.m_neighbourMap.size(), 2);
  QCOMPARE(ntlayer3.m_neighbourMap.size(), 2);

  QCOMPARE(ntlayer1.m_routingTable.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer1.m_routingTable.m_neighbourMap.contains(ntlayer2.m_address));
  QVERIFY(ntlayer1.m_routingTable.m_neighbourMap.contains(ntlayer3.m_address));
  QCOMPARE(ntlayer2.m_routingTable.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer2.m_routingTable.m_neighbourMap.contains(ntlayer1.m_address));
  QVERIFY(ntlayer2.m_routingTable.m_neighbourMap.contains(ntlayer3.m_address));
  QCOMPARE(ntlayer3.m_routingTable.m_neighbourMap.size(), 2);
  QVERIFY(ntlayer3.m_routingTable.m_neighbourMap.contains(ntlayer1.m_address));
  QVERIFY(ntlayer3.m_routingTable.m_neighbourMap.contains(ntlayer2.m_address));

  QCOMPARE(ntlayer1.m_routingTable.m_routerInfoMap.size(), 2);
  QVERIFY(ntlayer1.m_routingTable.m_routerInfoMap.contains(ntlayer2.m_address));
  QVERIFY(ntlayer1.m_routingTable.m_routerInfoMap.contains(ntlayer3.m_address));
  QCOMPARE(ntlayer2.m_routingTable.m_routerInfoMap.size(), 2);
  QVERIFY(ntlayer2.m_routingTable.m_routerInfoMap.contains(ntlayer1.m_address));
  QVERIFY(ntlayer2.m_routingTable.m_routerInfoMap.contains(ntlayer3.m_address));
  QCOMPARE(ntlayer3.m_routingTable.m_routerInfoMap.size(), 2);
  QVERIFY(ntlayer3.m_routingTable.m_routerInfoMap.contains(ntlayer1.m_address));
  QVERIFY(ntlayer3.m_routingTable.m_routerInfoMap.contains(ntlayer2.m_address));

  auto r1_2 = ntlayer1.m_routingTable.m_routerInfoMap[ntlayer2.m_address];
  QCOMPARE(r1_2.m_knowsInfoSet.size(), 2);

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
  QCOMPARE(ntlayerB.m_neighbourMap.size(), 2);
  QCOMPARE(ntlayerC.m_neighbourMap.size(), 1);

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
}
