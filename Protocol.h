#pragma once
#include <stdint.h>

enum class Protocol : int32_t
{
	Error,
	RoomList,
};

#include "arSocket.h"
class SocketBufferProtocol
	: private SocketBuffer
{
public:
	using Length = SocketBuffer::Length;
	static constexpr size_t socketSize = SocketBuffer::socketSize;
	static constexpr size_t lengthSize = SocketBuffer::lengthSize;
	static constexpr size_t protocolSize = sizeof(Protocol);
	static constexpr size_t bufferSize = SocketBuffer::bufferSize - protocolSize;
	static constexpr auto LengthToN = SocketBuffer::hton;
	static constexpr auto LengthToH = SocketBuffer::ntoh;
	static constexpr auto ProtocolToN = htonl;
	static constexpr auto ProtocolToH = ntohl;


public:
	SocketBufferProtocol();
	SocketBufferProtocol(Protocol protocol, const char* data, Length dataLength);

	void SetDataLength(Length dataLength);
	void SetProtocol(Protocol protocol);
	char* Buffer();
	char& operator[](Length index);

	Length GetDataLength() const;
	Protocol GetProtocol() const;
	const char* Buffer() const;
	const char& operator[](Length index) const;
};
