#include "Protocol.h"

//
//SocketBufferProtocol::SocketBufferProtocol() : SocketBuffer() {}
//SocketBufferProtocol::SocketBufferProtocol(const char * data, Length dataLength)
//{
//	SetDataLength(dataLength);
//	memcpy(, data, dataLength);
//}
//
//inline void SocketBufferProtocol::SetDataLength(Length dataLength) { SocketBuffer::SetDataLength(dataLength + protocolSize); }
//inline void SocketBufferProtocol::SetProtocol(Protocol protocol) {  }
//inline char* SocketBufferProtocol::Buffer() { return m_buffer; }
//inline char& SocketBufferProtocol::operator[](Length index) { return m_buffer[index]; }
//
//inline SocketBufferProtocol::Length SocketBufferProtocol::GetDataLength() const { return ntoh(m_dataLength) - lengthSize; }
//inline Protocol GetProtocol(Protocol protocol) {  }
//const char * SocketBufferProtocol::Buffer() const { return m_buffer; }
//const char & SocketBufferProtocol::operator[](Length index) const { return m_buffer[index]; }
