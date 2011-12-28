#include "testunidirectionalcable.h"


void TestUnidirectionalCable::testGoodCable()
{
  UnidirectionalCable cable(0.0);
  for (Byte i = 0; i < 255; i++)
    cable.write(i);

  for (Byte i = 0; i < 255; i++)
    QCOMPARE(cable.read(), i);

}


void TestUnidirectionalCable::testBadCable()
{
  UnidirectionalCable cable(1.0);

  for (Byte i = 0; i < 255; i++)
    cable.write(i);

  for (Byte i = 0; i < 255; i++)
  {
    Byte byte = cable.read();
    QCOMPARE((Byte)(byte ^ 255), i);
  }

}

void TestUnidirectionalCable::testNormalCable()
{
  UnidirectionalCable cable(0.1);

  for (Byte i = 0; i < 255; i++)
    cable.write(i);

  int goodCount = 0;
  int badCount = 0;
  for (Byte i = 0; i < 255; i++)
  {
    Byte byte = cable.read();
    if (byte == i)
      goodCount++;
    else
      badCount++;
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
