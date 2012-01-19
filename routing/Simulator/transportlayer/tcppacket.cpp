#include "tcppacket.h"

TCPPacket::TCPPacket():
  m_sourcePort(0), m_destinationPort(0), m_sequenceNumber(0),
  m_ackNumber(0), m_ackFlag(0), m_rstFlag(0), m_synFlag(0), m_finFlag(0),
  m_zero(0), m_windowSize(0), m_dataLength(0), m_data(NULL)
{
}

TCPPacket TCPPacket::fromBytes(BytePtr bytes)
{
  TCPPacket *pointer = (TCPPacket*) bytes.get();
  TCPPacket packet;
  memcpy((void *) &packet, (void *) pointer, headerLength());
  packet.m_data = bytes.get() + headerLength();
  return packet;
}

uint TCPPacket::toBytes(BytePtr &bytes)
{
  Byte *pointer = new Byte[length()];
  bytes = BytePtr(pointer, sharedArrayDeleter<Byte>);
  memcpy((void *) pointer, (void *) this, headerLength());
  memcpy((void *) (pointer + headerLength()), m_data, m_dataLength);
  return length();
}

uint TCPPacket::headerLength()
{
  return sizeof(TCPPacket) - sizeof(m_data);
}

uint TCPPacket::length()
{
  return headerLength() + m_dataLength;
}
