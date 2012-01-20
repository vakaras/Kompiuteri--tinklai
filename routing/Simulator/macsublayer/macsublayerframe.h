#ifndef MACSUBLAYERFRAME_H
#define MACSUBLAYERFRAME_H


#include <interfaces/IMACSublayer.h>

extern uint MAC_FRAME_DATA_SIZE_MAX;
extern uint MAC_FRAME_SIZE_MIN;

struct MACFrame
{

  _M IMACSublayer::Address  m_destinationAddress;
  _M IMACSublayer::Address  m_sourceAddress;
  _M ushort                 m_length;
  _M Byte*                  m_data;

  MACFrame();
  MACFrame(IMACSublayer::Address destinationAddress,
           IMACSublayer::Address sourceAddress,
           ushort length,
           Byte *data);

} __attribute__((packed));

#endif // MACSUBLAYERFRAME_H
