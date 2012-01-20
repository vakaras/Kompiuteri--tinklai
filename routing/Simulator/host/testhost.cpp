#include "testhost.h"
#include <router/router.h>
#include <host/smtpserver.h>
#include <cstring>

void TestHost::testInit()
{

  /*
    A -1- R -2- B
    */
  Byte data1[] = {2, 7, 1, 8, 2, 8, 1, 8, 2, 8, 4, 5, 9, 0, 4, 5};
  auto send = [&](Host *self)
  {
    TransportLayer *tlayer = self->transportLayer();
    ISocketPtr socket = tlayer->connect(3, 20);
    QVERIFY(socket);
    QCOMPARE(socket->isConnected(), true);
    qDebug() << "Sending data.";
    QCOMPARE(socket->send(data1, 16), true);
  };
  auto listen = [&](Host* self)
  {
    TransportLayer *tlayer = self->transportLayer();
    IListenerPtr listener = tlayer->createListener(20);
    ISocketPtr socket = listener->get();
    QCOMPARE(socket->isConnected(), true);
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
  };

  Cable cable1, cable2;
  Host a(1, send), b(3, listen);
  Router router(2);
  a.connect(cable1, 1);
  router.connect(cable1, 2);
  router.connect(cable2, 3);
  b.connect(cable2, 4);
  QTest::qWait(5000);
  a.start();
  b.start();
  qDebug() << "RUN";
  a.wait();
  b.wait();

}

void TestHost::testSMTP()
{
  auto server = [&](Host *self){
    SMTPServer server(self);
    server.run();
  };
  auto client = [&](Host *self){
    TransportLayer *tlayer = self->transportLayer();
    ISocketPtr socket = tlayer->connect(1, 25);
    QVERIFY(socket);
    char *data = "HELO astrauskas.lt\n"
                 "MAIL FROM:<vytautas@astrauskas.lt>\n"
                 "RCPT TO:<bla@gmail.com>\n"
                 "DATA\nSome text…\nOther Line\n\nSomething.\n.\n"
                 "QUIT\n";
    uint len = strlen(data);
    QCOMPARE(socket->send((Byte*) data, len), true);
  };
  Cable cable1, cable2;
  Host a(1, server);
  Host b(3, client);
  Router router(2);
  a.connect(cable1, 1);
  router.connect(cable1, 2);
  router.connect(cable2, 3);
  b.connect(cable2, 4);
  QTest::qWait(5000);
  a.start();
  b.start();
  qDebug() << "RUN";
  QTest::qWait(1000);
  a.stop();
  a.wait();
  b.wait();

}
