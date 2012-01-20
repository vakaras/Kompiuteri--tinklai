#include "smtpclient.h"
#include <cstring>

#define SEND(msg) if (!send(socket, msg)) return false; qDebug() << "SEND:" << msg;
#define CHECK(code) \
  if (!check(socket, code)) {\
    qDebug() << "FAILED CHECK:" << code;\
    return false;\
  }\
  qDebug() << "CHECK:" << code;

SMTPClient::SMTPClient(Host *host, ITransportLayer::Address serverAddress,
                       ITransportLayer* transportLayer,
                       QObject *parent):
  QObject(parent), m_host(host), m_serverAddress(serverAddress),
  m_transportLayer(transportLayer)
{
}

bool SMTPClient::send()
{
  m_buffer.clear();
  ITransportLayer* transportLayer = m_host->transportLayer();
  ISocket* socket = transportLayer->connect(m_serverAddress, 25);
  if (!socket || !socket->isConnected())
  {
    return false;
  }
  else
  {
    SEND("HELO astrauskas.lt\n");
    CHECK(220);
    SEND("MAIL FROM:<vytautas@astrauskas.lt>\n");
    CHECK(250);
    SEND("RCPT TO:<bla@gmail.com>\n");
    CHECK(250);
    SEND("DATA\n");
    CHECK(354);
    SEND("Some text…\nOther Line\n\nSomething.\n.\n");
    CHECK(250);
    SEND("QUIT\n");
    CHECK(221);
    qDebug() << "Client: remove socket.";
    m_transportLayer->remove(socket);
    return true;
  }
}

bool SMTPClient::send(ISocket *socket, const char *msg)
{
  uint len = strlen(msg);
  return socket->send((const Byte*) msg, len);
}

bool SMTPClient::check(ISocket *socket, uint code)
{
  int index = m_buffer.indexOf('\n');
  while (index == -1)
  {
    BytePtr bytes;
    uint len = socket->receive(bytes);
    for (uint i = 0; i < len; i++)
    {
      m_buffer.append((char) bytes.get()[i]);
    }
    index = m_buffer.indexOf('\n');
  }
  QString line = m_buffer;
  line.remove(index, line.size());
  m_buffer.remove(0, index+1);
  return line.startsWith(QString::number(code));
}
