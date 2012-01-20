#include "smtpserver.h"
#include <utils/executor.h>

SMTPServer::SMTPServer(Host *host, QObject *parent) :
    QObject(parent), m_host(host)
{
}

SMTPServer::~SMTPServer()
{
}

void SMTPServer::run()
{
  TransportLayer *transportLayer = m_host->transportLayer();
  IListenerPtr listener = transportLayer->createListener(25);

  while (m_host->go())
  {
    ISocketPtr socket = listener->get(1000);
    if (socket && socket->isConnected())
    {
      SMTPServerThreadPtr thread(new SMTPServerThread(socket, this,
                                                      transportLayer));
      thread->start();
      m_threads.append(thread);
    }
    // Remove old threads.
    for (int i = m_threads.size() - 1; i >= 0; i--)
    {
      if (m_threads.at(i)->isFinished())
      {
        m_threads.removeAt(i);
      }
    }
  }
  for (auto thread : m_threads)
  {
    thread->wait();
  }
}

void SMTPServer::append(SMTPMessage message)
{
  QMutexLocker locker(&m_serverMutex);
  m_inbox.append(message);
}

SMTPServer::MessageList SMTPServer::inbox()
{
  QMutexLocker locker(&m_serverMutex);
  return m_inbox;
}
