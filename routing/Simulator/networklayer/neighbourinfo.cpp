#include "neighbourinfo.h"
#include <QDateTime>

NeighbourInfo::NeighbourInfo():
  m_MACAddress(0), m_IPAddress(0), m_distance(UINT_MAX), m_expires(0)
{
}

NeighbourInfo::NeighbourInfo(IMACSublayer::Address MACAddress,
                             ILLCSublayerPtr layer,
                             INetworLayer::Address IPAdress, uint distance):
  m_MACAddress(MACAddress), m_connection(layer), m_IPAddress(IPAdress),
  m_distance(distance),
  m_expires(QDateTime::currentMSecsSinceEpoch() + NEIGHBOUR_INFO_LIFETIME)
{
}
