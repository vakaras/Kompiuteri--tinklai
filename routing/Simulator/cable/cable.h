#ifndef CABLE_H
#define CABLE_H

#include <QObject>
#include <types.h>
#include <cable/connectionpoint.h>
#include <cable/cableprocess.h>


class Cable : public QObject
{

  Q_OBJECT

private:

  _T ConnectionPoint*             ConnectionPointPtr;
  _T QList<ConnectionPointPtr>    ConnectionPointList;
  _M ConnectionPointList          m_connectionPoints;
  _M double                       m_errorRate;
  _M CableProcess                 m_process;

  /// Locks all connection points.
  _M void                   lockAll();
  /// Unlocks all connection points.
  _M void                   unlockAll();

public:

  explicit Cable(double errorRate=0.0, ulong sleepTime=0,
                 QObject *parent = 0);
  _M Vacuum                 ~Cable();

  /// Processes one bit writing cycle.
  _M void                   processCycle();
  _M ConnectionPoint*       createConnectionPoint();

};

#endif // CABLE_H
