#include "executor.h"

Executor::Executor(VoidFunction function, QObject *parent) :
    QThread(parent), m_function(function)
{
}

void Executor::run()
{
  m_function();
}
