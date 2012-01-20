#ifndef SMTPSERVER_H
#define SMTPSERVER_H

#include <host/host.h>
#include <QMultiMap>
#include <QString>
#include <QList>
#include <host/smtpserverthread.h>
#include <host/smtpmessage.h>

class SMTPServer : public QObject
{

  Q_OBJECT

public:

  _T QList<SMTPMessage>           MessageList;
  _T QList<SMTPServerThreadPtr>   ThreadList;

private:

  _M Host*        m_host;
  _M MessageList  m_inbox;
  _M QMutex       m_serverMutex;

  _M ThreadList   m_threads;

  _M void         parseClient(ISocketPtr socket);

public:
  explicit SMTPServer(Host *host, QObject *parent = 0);
  _M Vacuum       ~SMTPServer();
  _M void         run();
  _M void         append(SMTPMessage message);
  _M MessageList  inbox();
};

#endif // SMTPSERVER_H
