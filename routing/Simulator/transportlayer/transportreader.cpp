#include "transportreader.h"
#include <transportlayer/transportlayer.h>

TransportReader::TransportReader(
  INetworkLayer *networkLayer,
  TransportLayer *transportLayer,
  QObject *parent) :
  GenericThread(parent),
  m_transportLayer(transportLayer),
  m_networkLayer(networkLayer)
{
}

void TransportReader::run()
{
  m_goMutex.lock();
  while (m_go)
  {
    m_goMutex.unlock();
    INetworkLayer::Address address;
    BytePtr bytes;
    uint len = m_networkLayer->receive(address, bytes, 1000);
    if (len)
    {
      m_transportLayer->parseSegment(address, bytes, len);
    }
    m_goMutex.lock();
  }
  m_goMutex.unlock();
}
