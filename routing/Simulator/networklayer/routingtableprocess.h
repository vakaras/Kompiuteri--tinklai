#ifndef ROUTINGTABLEPROCESS_H
#define ROUTINGTABLEPROCESS_H

#include <utils/genericthread.h>
#include <QWaitCondition>


class RoutingTable;

class RoutingTableProcess : public GenericThread
{

  Q_OBJECT

private:

  _M QWaitCondition         m_waitCondition;
  _M RoutingTable*          m_table;
  _M void                   run();

public:
  explicit RoutingTableProcess(RoutingTable *table, QObject *parent = 0);
  _V void                   stop();
  _V void                   wake();
};

#endif // ROUTINGTABLEPROCESS_H
