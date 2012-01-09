#ifndef CONNECTIONWRAPPER_H
#define CONNECTIONWRAPPER_H

#include <interfaces/ILLCSublayer.h>
#include <networklayer/networkreader.h>
// TODO NetworkWriter


class NetworkLayer;

class ConnectionWrapper
{
private:

  _M NetworkLayer*      m_layer;
  _M ILLCSublayerPtr    m_connection;

  _M NetworkReaderPtr   m_reader;

public:

  _M Vacuum             ConnectionWrapper(NetworkLayer* layer,
                                          ILLCSublayerPtr connection);
  _M Vacuum             ~ConnectionWrapper();

  _Y ILLCSublayerPtr    connection() const;
};

#endif // CONNECTIONWRAPPER_H
