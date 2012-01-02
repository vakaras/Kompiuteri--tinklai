#include "random.h"
#include <QMutex>
#include <QMutexLocker>
#include <cstdlib>


QMutex mutex;

uint random(uint maxRand)
{
  QMutexLocker locker(&mutex);
  double number = ((double) rand()) / RAND_MAX;
  return (uint) (number * maxRand);
}
