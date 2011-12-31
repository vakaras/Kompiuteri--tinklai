#include "testdatalink.h"
#include "src/unidirectionalcable/unidirectionalcable.h"


void TestDataLink::testInit()
{
  return;
  UnidirectionalCable cable1(0.1);
  UnidirectionalCable cable2(0.1);
  DataLink link1(&cable1, &cable2);
  DataLink link2(&cable2, &cable1);

  QVERIFY(link1.m_connectionIn == &cable1);
  QVERIFY(link1.m_connectionOut == &cable2);
  QVERIFY(link2.m_connectionIn == &cable2);
  QVERIFY(link2.m_connectionOut == &cable1);
}

void TestDataLink::testFrameHeader()
{
  return;
  DataLink::FrameHeader header;
  QVERIFY(sizeof(header) == 2);
  QVERIFY(sizeof(header.informationControl) == 1);
  QVERIFY(sizeof(header.supervisoryControl) == 1);
  header.address = 0;
  header.informationControl.frameType = 0;      // Information frame.
  header.informationControl.seq = 5;
  header.informationControl.pf = 0;
  header.informationControl.next = 2;

  uint *p = (uint *) &header;
  //qDebug("%x", *p);

  QVERIFY(header.address == 0);
  QVERIFY(header.supervisoryControl.frameType == 0);
  QVERIFY(header.supervisoryControl.zero == 1);
  QVERIFY(header.supervisoryControl.type == 2);
  QVERIFY(header.supervisoryControl.pf == 0);
  QVERIFY(header.supervisoryControl.next == 2);

  *p = 0x8CEF;
//qDebug("%x", *p);
  header = *((DataLink::FrameHeader *) p);
//qDebug("Address:    %x", header.address); // EF
//qDebug("Frame type: %x", header.informationControl.frameType);
//qDebug("Seq:        %x", header.informationControl.seq);
//qDebug("p/f:        %x", header.informationControl.pf);
//qDebug("Next:       %x", header.informationControl.next);
  /*
    | E     | F     | 8         | C             |
    | 1110  | 1111  | 100   0   | 110 0         |
    | address       | next  p/f | seq frameType |
    */
  QVERIFY(header.address == 0xEF);
  QVERIFY(header.informationControl.frameType == 0);
  QVERIFY(header.informationControl.seq == 6);
  QVERIFY(header.informationControl.pf == 0);
  QVERIFY(header.informationControl.next == 4);
}

void TestDataLink::testEncodingFrame()
{
  return;
  UnidirectionalCable cable1(0.0, 0.0);
  UnidirectionalCable cable2(0.0, 0.0);
  DataLink link(&cable1, &cable2);

  DataLink::FrameHeader header;
  ushort *p = (ushort *) &header;
  *p = 0x8CEF;

  Byte data[] = {0xFF, 0xFF, 0xFF};
  link.writeFrame(&header, data, 3);

  Bit buffer[] = {
    0, 1, 1, 1, 1, 1, 1, 0,                 // FLAG
    1, 1, 1, 1, 0, 1, 1, 1,                 // FE
    0, 0, 1, 1, 0, 0, 0, 1,                 // C8
    1, 1, 1, 1, 0, 1, 1, 1, 1,
    1, 0, 1, 1, 1, 1, 1, 0, 1, 1,
    1, 1, 1, 0, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 0, 0,              // CheckSum
    0, 1, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 0,                 // FLAG
  };
  int counter = 0;
  for (auto bit : buffer)
  {
    if (bit != cable2.read())
    {
      qDebug() << "Counter: " << counter << (counter >> 3) << (counter & 0x7);
      qDebug() << "Bit: " << bit;
      QFAIL("Output differ.");
    }
    counter++;
  }
}

void TestDataLink::testDecodingFrame()
{
  return;
  UnidirectionalCable cable1(0.0, 0.0);
  UnidirectionalCable cable2(0.0, 0.0);
  DataLink link1(&cable1, &cable2);
  DataLink link2(&cable2, &cable1);

  DataLink::FrameHeader header;
  ushort *p = (ushort *) &header;
  *p = 0x8CEF;

  QVERIFY(link2.m_receivedDataBuffer.isEmpty());
  Byte data[] = {0xAF, 0xDE, 0xAD, 0xBA, 0xBA, 0x3F};
  link1.writeFrame(&header, data, 6);

  Byte dataReceived[100];
  QCOMPARE(link2.read(dataReceived, 100, 200), 0);

  header.informationControl.seq = 0;
  link1.writeFrame(&header, data, 6);
  QCOMPARE(link2.read(dataReceived, 100, 200), 6);
  for (int i = 0; i < 6; i++)
    QCOMPARE(dataReceived[i], data[i]);
  QTest::qWait(500);
  QCOMPARE(link1.m_writeWindowLowerBound, 1u);
}

void TestDataLink::testSendingData()
{
  UnidirectionalCable cable1(0.0, 0.0);
  UnidirectionalCable cable2(0.0, 0.0);
  DataLink link1(&cable1, &cable2);
  DataLink link2(&cable2, &cable1);

  Byte data[] = {0xAF, 0xDE, 0xAD, 0xBA, 0xBA, 0x3F};
  QCOMPARE(link1.write(data, 6), true);
}
