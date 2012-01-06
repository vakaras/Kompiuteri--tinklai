#include "llcsublayer.h"

LLCSublayer::LLCSublayer(IMACSublayer *mac, QObject *parent) :
    QObject(parent), m_mac(mac)
{
}

bool LLCSublayer::send(IMACSublayer::Address address, Byte *bytes, uint len)
{
  return m_mac->write(address, bytes, len);
}

bool LLCSublayer::broadcast(Byte *bytes, uint len)
{
  return m_mac->write(MAC_BROADCAST_ADDRESS, bytes, len);
}

uint LLCSublayer::receive(IMACSublayer::Address &address, BytePtr &bytes,
                          ulong time)
{
  return m_mac->read(address, bytes, time);
}

void LLCSublayer::reconnect()
{
  m_mac->reconnect();
}
