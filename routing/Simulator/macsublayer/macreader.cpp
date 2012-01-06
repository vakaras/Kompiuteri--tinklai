#include "macreader.h"
#include <macsublayer/macsublayer.h>

#define CHANGE_GO(value) m_goMutex.lock(); m_go = value; m_goMutex.unlock();

MACReader::MACReader(MACSublayer *layer, IPhysicalConnectionPtr connection,
                     QObject *parent) :
    QThread(parent), m_connection(connection), m_layer(layer), m_go(false)
{
}

void MACReader::start()
{
  CHANGE_GO(true);
  QThread::start();
}

void MACReader::stop()
{
  CHANGE_GO(false);
  wait();
}

void MACReader::analyseBitChange(Bit bit)
{
  if (bit == 0)
  {
    if (m_count[1] == 5 && !m_buffer.isEmpty())
    {
      // Removing stuffed bit.
      m_buffer.removeLast();
    }
    else if (m_count[1] == 6 && m_buffer.size() >= 8 &&
             m_buffer.at(m_buffer.size() - 8) == 0)
    {
      // We have found marker: 0111 1110.
      markerFoundAction();
    }
    else if (m_count[1] >= 6)
    {
      // Damaged frame.
      m_state = Search;
    }
  }
  m_count[bit] = 1;
}

void MACReader::markerFoundAction()
{
  if (m_state == Search)
  {
    // Frame begins.
    m_state = Read;
    m_buffer.clear();
  }
  else // m_state == Read
  {
    // One frame ends, other begins.
    for (int i = 0; i < 8; i++)
    {
      // Remove marker byte.
      if (!m_buffer.isEmpty())
        m_buffer.removeLast();
    }
    m_layer->save(m_buffer);
    m_state = Read;
    m_buffer.clear();
  }
}

void MACReader::run()
{
  m_state = Search;
  m_buffer.clear();
  m_count[1] = 0; // How many 1 found.
  m_count[0] = 0; // How many 0 found.
  Bit lastBit = 0;

  m_goMutex.lock();
  while (m_go)
  {
    m_goMutex.unlock();
    bool timedOut = false;
    Bit bit = m_connection->read(100, &timedOut);
    if (!timedOut)
    {
      m_buffer.append(bit);
      if (lastBit == bit)
        m_count[bit]++;
      else
      {
        analyseBitChange(bit);
      }
      lastBit = bit;
    }
    m_goMutex.lock();
  }
  m_goMutex.unlock();
}
