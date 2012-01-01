#include "macsublayerframe.h"

MACFrame::MACFrame():
  m_destinationAddress(0),
  m_sourceAddress(0),
  m_length(0),
  m_data(0)
{
}

MACFrame::MACFrame(
  IMACSublayer::Address destinationAddress,
  IMACSublayer::Address sourceAddress,
  ushort length, Byte *data):
  m_destinationAddress(destinationAddress),
  m_sourceAddress(sourceAddress),
  m_length(length),
  m_data(data)
{
}
