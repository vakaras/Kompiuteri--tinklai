#ifndef SMTPSERVERTHREAD_H
#define SMTPSERVERTHREAD_H

#include <utils/genericthread.h>
#include <interfaces/ITransportLayer.h>
#include <host/smtpserverthread.h>
#include <host/smtpmessage.h>


class SMTPServer;

class SMTPServerThread : public GenericThread
{

  Q_OBJECT

private:

  _M ISocketPtr   m_socket;
  _M SMTPServer*  m_server;
  _M QString      m_buffer;

  _M void         run();
  _M QString      readLine();
  _M QString      readMessage();
  _M void         send(const char *msg);

public:

  explicit SMTPServerThread(ISocketPtr socket, SMTPServer* server,
                            QObject *parent = 0);
};

typedef std::shared_ptr<SMTPServerThread>   SMTPServerThreadPtr;

#endif // SMTPSERVERTHREAD_H
