#ifndef NETWORKLAYER_TYPES_H
#define NETWORKLAYER_TYPES_H

#include <types.h>

enum class NetworkLayerFrameType : Byte
{
  Unset,
  HelloRequest,
  HelloAnswer,
  NeighbourInfo,
  NeighbourInfoACK
};

#endif // NETWORKLAYER_TYPES_H
