#include "routerinfo.h"

RouterInfo::RouterInfo():
  m_sequenceNumber(0), m_expires(0), m_distance(UINT_MAX)
{
}

RouterInfo::RouterInfo(INetworLayer::Address ipAddress) :
  m_ipAddress(ipAddress), m_sequenceNumber(0), m_expires(0),
  m_distance(UINT_MAX)
{
}
