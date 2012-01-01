#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QThread>
#include <functional>
#include <types.h>

class Executor : public QThread
{

  Q_OBJECT

private:

  _T std::function<void ()>   VoidFunction;
  _M VoidFunction             m_function;

protected:

  _M void                     run();

public:

  explicit Executor(VoidFunction function, QObject *parent = 0);

};

#endif // EXECUTOR_H
