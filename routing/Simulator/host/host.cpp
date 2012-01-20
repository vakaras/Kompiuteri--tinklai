#include "host.h"

Host::Host(INetworkLayer::Address address,
           VoidFunction function, QObject *parent) :
  GenericThread(parent), m_function(function), m_address(address),
  m_networkLayer(address), m_transportLayer(&m_networkLayer),
  m_lastConnectionId(0)
{
}

Host::~Host()
{
  GenericThread::stop();
}

void Host::run()
{
  m_function(this);
}

uint Host::connect(Cable &cable, IMACSublayer::Address address)
{
  m_lastConnectionId++;
  m_macs[m_lastConnectionId] = IMACSublayerPtr(
        new MACSublayer(address, cable.createConnectionPoint()));
  m_llcs[m_lastConnectionId] = ILLCSublayerPtr(
        new LLCSublayer(m_macs[m_lastConnectionId].get()));
  m_networkLayer.append(m_llcs[m_lastConnectionId]);
  return m_lastConnectionId;
}

bool Host::disconnect(uint connectionId)
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

TransportLayer* Host::transportLayer()
{
  return &m_transportLayer;
}
