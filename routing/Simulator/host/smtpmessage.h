#ifndef SMTPMESSAGE_H
#define SMTPMESSAGE_H

#include <types.h>
#include <QString>
#include <QStringList>
#include <QList>

struct SMTPMessage
{

  _M QString      m_from;
  _M QStringList  m_to;
  _M QString      m_body;

  SMTPMessage();
};

#endif // SMTPMESSAGE_H
