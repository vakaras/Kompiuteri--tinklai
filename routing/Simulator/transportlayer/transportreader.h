#ifndef TRANSPORTREADER_H
#define TRANSPORTREADER_H

#include <utils/genericthread.h>
#include <interfaces/INetworkLayer.h>


class TransportLayer;

class TransportReader : public GenericThread
{

  Q_OBJECT

private:

  _M TransportLayer*  m_transportLayer;
  _M INetworkLayer*   m_networkLayer;

  _M void             run();

public:
  explicit TransportReader(INetworkLayer *networkLayer,
                           TransportLayer *transportLayer,
                           QObject *parent = 0);
};

#endif // TRANSPORTREADER_H
