#ifndef TCPPACKET_H
#define TCPPACKET_H

#include <interfaces/ITransportLayer.h>

struct TCPPacket
{

  _M ITransportLayer::Port  m_sourcePort;
  _M ITransportLayer::Port  m_destinationPort;
  _M uint                   m_sequenceNumber;
  _M uint                   m_ackNumber;
  _M uint                   m_ackFlag : 1;
  _M uint                   m_rstFlag : 1;
  _M uint                   m_synFlag : 1;
  _M uint                   m_finFlag : 1;
  _M uint                   m_zero    : 12;
  _M ushort                 m_windowSize;
  _M uint                   m_dataLength;
  _M Byte*                  m_data;

  _M Vacuum                 TCPPacket();
  _S TCPPacket              fromBytes(BytePtr bytes);
  _M uint                   toBytes(BytePtr &bytes);
  _S uint                   headerLength();
  _M uint                   length();

} __attribute__((packed));

#endif // TCPPACKET_H
