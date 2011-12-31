#ifndef CABLE_H
#define CABLE_H

#include <QObject>
#include <types.h>
#include <cable/connectionpoint.h>


class Cable : public QObject
{

  Q_OBJECT

private:

  _T QList<ConnectionPoint> ConnectionPointList;
  _M ConnectionPointList    m_connectionPoints;
  _M double                 m_errorRate;

  /// Locks all connection points.
  _M void                   lockAll();
  /// Unlocks all connection points.
  _M void                   unlockAll();

public:

  explicit Cable(double errorRate=0.0, QObject *parent = 0);

  /// Processes one bit writing cycle.
  _M void                   processCycle();
  _M ConnectionPoint*       createConnectionPoint();

};

#endif // CABLE_H
