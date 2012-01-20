#include "smtpserverthread.h"
#include <host/smtpmessage.h>
#include <QString>
#include <QStringList>
#include <cstring>
#include <host/smtpserver.h>

SMTPServerThread::SMTPServerThread(ISocketPtr socket, SMTPServer *server,
                                   ITransportLayer* transportLayer,
                                   QObject *parent):
  GenericThread(parent), m_transportLayer(transportLayer),
  m_socket(socket), m_server(server)
{
}

void SMTPServerThread::run()
{
  qDebug() << "    Starting thread.";
  send("220 server SMTP\n");
  QString line = readLine();
  qDebug() << "    Received line:" << line;
  if (line.startsWith("HELO "))
  {
    SMTPMessage message;
    while (true)
    {
      line = readLine();
      qDebug() << "    Received line:" << line;
      if (line.startsWith("MAIL FROM:"))
      {
        message.m_from = line.remove(0, strlen("MAIL FROM:"));
        qDebug() << "FROM:" << message.m_from;
        send("250 Ok\n");
      }
      else if (line.startsWith("RCPT TO:"))
      {
        message.m_to.append(line.remove(0, strlen("RCPT TO:")));
        qDebug() << "TO:" << message.m_to;
        send("250 Ok\n");
      }
      else if (line.startsWith("DATA"))
      {
        send("354 End data with <LF>.<LF>\n");
        message.m_body = readMessage();
        if (!message.m_body.isEmpty())
        {
          qDebug() << "MSG:" << message.m_body;
          m_server->append(message);
          send("250 Ok\n");
        }
      }
      else if (line.startsWith("QUIT"))
      {
        send("221 Bye.\n");
        break;
      }
      else if (!m_socket->isConnected())
      {
        break;
      }
    }
  }
  else
  {
    send("221 Error, bye.\n");
  }
  m_transportLayer->remove(m_socket);

}

QString SMTPServerThread::readLine()
{
  int index = m_buffer.indexOf('\n');
  while (index == -1)
  {
    BytePtr bytes;
    uint len = m_socket->receive(bytes);
    for (uint i = 0; i < len; i++)
    {
      m_buffer.append((char) bytes.get()[i]);
    }
    index = m_buffer.indexOf('\n');
  }
  QString line = m_buffer;
  line.remove(index, line.size());
  m_buffer.remove(0, index+1);
  return line;
}

QString SMTPServerThread::readMessage()
{
  int index = m_buffer.indexOf("\n.\n");
  while (index == -1)
  {
    BytePtr bytes;
    uint len = m_socket->receive(bytes);
    for (uint i = 0; i < len; i++)
    {
      m_buffer.append((char) bytes.get()[i]);
    }
    index = m_buffer.indexOf("\n.\n");
  }
  QString line = m_buffer;
  line.remove(index, line.size());
  m_buffer.remove(0, index+3);
  return line;
}

void SMTPServerThread::send(const char *msg)
{
  uint len = strlen(msg);
  m_socket->send((Byte*) msg, len);
}
