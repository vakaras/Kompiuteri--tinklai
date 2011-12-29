#include "testunidirectionalcable.h"


void TestUnidirectionalCable::testGoodCable()
{
  UnidirectionalCable cable(0.0, 0.0);
  for (Byte i = 0; i < 255; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      cable.write((i >> j) & 1);
    }
  }

  for (Byte i = 0; i < 255; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      QCOMPARE(cable.read(), (i >> j) & 1);
    }
  }

}


void TestUnidirectionalCable::testBadCable()
{
  UnidirectionalCable cable(1.0, 0.0);

  for (Byte i = 0; i < 255; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      cable.write((i >> j) & 1);
    }
  }

  for (Byte i = 0; i < 255; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      QCOMPARE(cable.read() ^ 1, (i >> j) & 1);
    }
  }

}

void TestUnidirectionalCable::testNormalCable()
{
  UnidirectionalCable cable(0.5, 0.5);

  for (Byte i = 0; i < 255; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      cable.write((i >> j) & 1);
    }
  }

  int goodCount = 0;
  int badCount = 0;
  for (Byte i = 0; i < 240; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      Bit bit = cable.read();
      if (bit == ((i >> j) & 1))
        goodCount++;
      else
        badCount++;
    }
  }

  QVERIFY(goodCount > 0);
  QVERIFY(badCount > 0);

}

void TestUnidirectionalCable::testEmptyCable()
{
  UnidirectionalCable cable;
  Reader reader(&cable);
  reader.start();
  cable.write(0);
  reader.wait();
}
