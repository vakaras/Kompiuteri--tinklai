#ifndef TESTUNIDIRECTIONALCABLE_H
#define TESTUNIDIRECTIONALCABLE_H

#include <QtTest/QTest>
#include <QThread>
#include <src/unidirectionalcable/unidirectionalcable.h>

class Reader : public QThread
{

  Q_OBJECT

private:

  UnidirectionalCable     *m_cable;

protected:

  _M void                 run()
  {
    m_cable->read();
  }

public:
  explicit Reader(UnidirectionalCable *cable = 0, QObject *parent = 0) :
    QThread(parent), m_cable(cable)
  {
  }

};

class TestUnidirectionalCable : public QObject
{

  Q_OBJECT

private:


private slots:

  _M void         testGoodCable();
  _M void         testBadCable();
  _M void         testNormalCable();
  _M void         testEmptyCable();

};

#endif // TESTUNIDIRECTIONALCABLE_H
