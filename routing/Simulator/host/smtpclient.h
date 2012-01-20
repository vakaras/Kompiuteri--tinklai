#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <host/host.h>
#include <interfaces/ITransportLayer.h>
#include <QObject>

class SMTPClient : public QObject
{

  Q_OBJECT

private:

  _M Host*                      m_host;
  _M ITransportLayer::Address   m_serverAddress;
  _M QString                    m_buffer;

  _M bool                       send(ISocket* socket, const char *msg);
  _M bool                       check(ISocket* socket, uint code);

public:

  explicit SMTPClient(Host *host, ITransportLayer::Address serverAddress,
                      QObject *parent = 0);
  _M bool     send();
};

#endif // SMTPCLIENT_H
