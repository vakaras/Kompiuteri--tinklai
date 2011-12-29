#include "datalink.h"

DataLink::DataLink(IPhysicalConnection *connectionIn,
                   IPhysicalConnection *connectionOut,
                   QObject *parent) :
  QObject(parent),
  m_connectionIn(connectionIn),
  m_connectionOut(connectionOut),
  m_reader(this)
{
  connectionIn->reset();
  connectionOut->reset();
  m_reader.start();
}

DataLink::~DataLink()
{
  m_reader.stop();
  m_reader.wait();
}

void DataLink::writeFrame(
  const FrameHeader *header, const Byte *bytes, uint len)
{
  Byte *buffer = new Byte[2 + len + 2];
  // header data checksum
  buffer[0] = ((Byte *) header)[0];
  buffer[1] = ((Byte *) header)[1];
  for (uint i = 0; i < len; i++)
  {
    buffer[i+2] = bytes[i];
  }
  ushort checkSum = qChecksum((const char *)buffer, len + 2);
  buffer[2+len] = checkSum & 0xff;
  buffer[3+len] = (checkSum >> 8) & 0xff;

  for (auto it : {0, 1, 1, 1, 1, 1, 1, 0})
    m_connectionOut->write(it);
  uint counter = 0;
  for (uint j = 0; j < len + 4; j++)
  {
    for (int i = 0; i < 8; i++)
    {
      Bit bit = (buffer[j] >> i) & 1;
      m_connectionOut->write(bit);
      if (bit == 1)
      {
        counter++;
        if (counter == 5)
        {
          m_connectionOut->write(0);
          counter = 0;
        }
      }
      else
      {
        counter = 0;
      }
    }
  }
  for (auto it : {0, 1, 1, 1, 1, 1, 1, 0})
    m_connectionOut->write(it);
  delete [] buffer;
}

void DataLink::parseFrame(QList<Bit> *buffer)
{
  qDebug() << "Parsing frame!";
  uint len = buffer->size();
  if ((len < 32) || (len & 0x7))
  {
    // Frame is damaged.
    return;
  }
  FrameHeader header;
  ushort *p = (ushort*) &header;
  *p = 0;

  for (int i = 0; i < 16; i++)
  {
    *p |= buffer->first() << i;
    buffer->removeFirst();
  }
  qDebug("%x", *p);

  int j = 0;
  Byte byte = 0;
  for (auto bit : *buffer)
  {
    byte |= bit << j;
    j++;
    if (j == 8)
    {
      qDebug("%x", byte);
      byte = 0;
      j = 0;
    }
  }

}

void DataLinkReader::run()
{
  qDebug() << "Start";
  State state = Search;
  QList<Bit> buffer;
  uint count[2];
  count[1] = 0;                         // How many 1 found.
  count[0] = 0;                         // How many 0 found.
  Bit lastBit = 0;
  while (m_go)
  {
    bool timedOut = false;
    Bit bit = m_link->m_connectionIn->read(100, &timedOut);
    if (timedOut)
    {
      continue;
    }
    qDebug() << "Bit: " << this << bit;
    buffer.append(bit);
    if (lastBit == bit)
    {
      count[bit]++;
    }
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
          else                          // state == Read
          {
            // One frame ends, other begins.
            for (int i = 0; i < 8; i++)
            {
              // Remove marker byte.
              buffer.pop_back();
            }
            m_link->parseFrame(&buffer);
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
