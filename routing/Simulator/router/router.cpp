#include "router.h"

Router::Router(INetworLayer::Address address, QObject *parent) :
  QObject(parent), m_address(address), m_networkLayer(address),
  m_lastConnectionId(0)
{
}

Router::~Router()
{
}

uint Router::connect(Cable &cable, IMACSublayer::Address address)
{
  m_lastConnectionId++;
  m_macs[m_lastConnectionId] = IMACSublayerPtr(
        new MACSublayer(address, cable.createConnectionPoint()));
  m_llcs[m_lastConnectionId] = ILLCSublayerPtr(
        new LLCSublayer(m_macs[m_lastConnectionId].get()));
  m_networkLayer.append(m_llcs[m_lastConnectionId]);
  return m_lastConnectionId;
}

bool Router::disconnect(uint connectionId)
{
  if (m_networkLayer.remove(m_llcs[connectionId]))
  {
    m_macs.remove(connectionId);
    m_llcs.remove(connectionId);
    return true;
  }
  else
  {
    return false;
  }
}

INetworLayer::Address Router::address()
{
  return m_address;
}

bool Router::send(Address address, Byte *bytes, uint len)
{
  return m_networkLayer.send(address, bytes, len);
}

uint Router::receive(Address &address, BytePtr &bytes, ulong time)
{
  return m_networkLayer.receive(address, bytes, time);
}

void Router::restart()
{
  m_networkLayer.restart();
}
