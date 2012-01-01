#include "macreader.h"
#include <macsublayer/macsublayer.h>

MACReader::MACReader(MACSublayer *layer, QObject *parent) :
    QThread(parent), m_layer(layer), m_go(false)
{
}

void MACReader::start()
{
  m_go = true;
  QThread::start();
}

void MACReader::stop()
{
  m_go = false;
  wait();
}

void MACReader::run()
{
  qDebug() << "Start";
  State state = Search;
  QList<Bit> buffer;
  uint count[2];
  count[1] = 0; // How many 1 found.
  count[0] = 0; // How many 0 found.
  Bit lastBit = 0;

  while (m_go)
  {
    bool timedOut = false;
    Bit bit = m_connection->read(100, &timedOut);
    if (timedOut)
      continue;
    buffer.append(bit);
    if (lastBit == bit)
      count[bit]++;
    else
    {
      if (bit == 0)
      {
        if (count[1] == 5)
        {
          // Removing stuffed bit.
          buffer.pop_back();
        }
        else if (count[1] == 6)
        {
          qDebug() << "Found marker";
          // We have found marker: 0111 1110.
          if (state == Search)
          {
            qDebug() << "Clear buffer. New frame begins.";
            // Frame begins.
            state = Read;
            buffer.clear();
          }
          else // state == Read
          {
            // One frame ends, other begins.
            for (int i = 0; i < 8; i++)
              // Remove marker byte.
              buffer.pop_back();
            m_layer->save(buffer);
            state = Read;
            buffer.clear();
          }
        }
        else if (count[1] > 6)
        {
          // Damaged frame.
          state = Search;
        }
      }
      count[bit] = 1;
    }
    lastBit = bit;
  }
}
