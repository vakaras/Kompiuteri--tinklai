#ifndef SHAREDPTRDELETER_H
#define SHAREDPTRDELETER_H

template<class T>
void sharedArrayDeleter(T *pointer)
{
  delete [] pointer;
}

#endif // SHAREDPTRDELETER_H
