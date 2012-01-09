#include "connectionwrapper.h"

ConnectionWrapper::ConnectionWrapper(NetworkLayer *layer,
                                     ILLCSublayerPtr connection):
  m_layer(layer), m_connection(connection)
{
  m_reader = NetworkReaderPtr(new NetworkReader(layer, connection));
  m_reader->start();
}

ConnectionWrapper::~ConnectionWrapper()
{
}

const ILLCSublayerPtr ConnectionWrapper::connection() const
{
  return m_connection;
}
