#ifndef MACREADER_H
#define MACREADER_H

#include <QThread>
#include <QMutex>
#include <interfaces/IPhysicalConnection.h>
#include <types.h>

class MACSublayer;

class MACReader : public QThread
{

  Q_OBJECT

private:

  _E State {
    Search,                             // Searching the beginning of the
                                        // next frame.
    Read                                // Reading frame.
    };
  _M IPhysicalConnectionPtr   m_connection;
  _M MACSublayer*             m_layer;
  _M bool                     m_go;
  _M QMutex                   m_goMutex;

  _M State                    m_state;
  _M QList<Bit>               m_buffer;
  _M uint                     m_count[2];

  _M void                     analyseBitChange(Bit bit);
  _M void                     markerFoundAction();

protected:

  _M void                     run();

public:

  explicit MACReader(MACSublayer *layer, IPhysicalConnectionPtr connection,
                     QObject *parent = 0);
  _M void                 start();
  _M void                 stop();

};

#endif // MACREADER_H
