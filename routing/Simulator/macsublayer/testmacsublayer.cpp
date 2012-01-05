#include "testmacsublayer.h"
#include <cable/cable.h>
#include <utils/executor.h>

#define N 10

void TestMACSublayer::testInit()
{
  Cable cable(0.0, 0);
  MACSublayer layer1(1, cable.createConnectionPoint());
  MACSublayer layer2(2, cable.createConnectionPoint());

  Byte data[] = {0xbb, 0xFF, 0xFF, 0xFF, 0xcc};
  QCOMPARE(layer1.write(2, data, 5), true);

  IMACSublayer::Address address;
  BytePtr bytes;
  QCOMPARE(layer2.read(address, bytes), 5u);
  QCOMPARE(address, 1u);
  int i = 0;
  for (auto it : data)
    QCOMPARE(*(bytes.get() + i++), it);
  QCOMPARE(layer2.read(address, bytes, 0), 0u);
  QCOMPARE(layer1.read(address, bytes, 0), 0u);

}

void TestMACSublayer::testDeletingCableClient()
{
  Cable cable(0.0, 0);
  MACSublayer *layer1 = new MACSublayer(1, cable.createConnectionPoint());
  MACSublayer *layer2 = new MACSublayer(2, cable.createConnectionPoint());

  Byte data[] = {0xbb, 0xFF, 0xFF, 0xFF, 0xcc};
  QCOMPARE(layer1->write(2, data, 5), true);
  delete layer1;

  IMACSublayer::Address address;
  BytePtr bytes;
  QCOMPARE(layer2->read(address, bytes), 5u);
  QCOMPARE(address, 1u);
  int i = 0;
  for (auto it : data)
    QCOMPARE(*(bytes.get() + i++), it);
  QCOMPARE(layer2->read(address, bytes, 0), 0u);
  delete layer2;
}

void TestMACSublayer::testCollision()
{
  Cable cable(0.0, 0);

  MACSublayer *layers[N];
  for (int i = 0; i < N; i++)
    layers[i] =  new MACSublayer(i, cable.createConnectionPoint());

  Byte data[200];
  for (uint i = 0; i < 200; i++)
    data[i] = i;

  Executor *writers[N];
  for (int i = 0; i < N; i++)
  {
    writers[i] = new Executor(
          [&, i]()
          {
            QCOMPARE(layers[i]->write(BROADCAST_ADDRESS, data, 200 - i), true);
          });
  }
  qDebug() << "Writers created.";
  for (auto writer : writers)
    writer->start();

  qDebug() << "Writers started.";
  for (auto writer : writers)
    writer->wait();
  qDebug() << "Writers finished.";

  for (auto layer : layers)
  {
    for (int i = 0; i < N; i++)
    {
      BytePtr bytes;
      IMACSublayer::Address address;
      uint len = layer->read(address, bytes);
      QCOMPARE(len + address, 200u);
      for (uint j = 0; j < len; j++)
      {
        QCOMPARE(bytes.get()[j], data[j]);
      }
    }
  }

  for (auto layer : layers)
    delete layer;
}
