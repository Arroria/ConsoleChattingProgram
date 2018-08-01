#pragma once
#include <winsock2.h>
#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable : 4996)


struct SocketError
{
	int errorCode;
	const char* errorName;
	SocketError() : errorCode(NULL), errorName(nullptr) {}
	SocketError(int _errorCode, const char* _errorName) : errorCode(_errorCode), errorName(_errorName) {}
};



bool __ar_WSAStartup(SocketError* errorReturn, WORD wVersionRequested, LPWSADATA lpWSAData);
bool __ar_socket(SocketError* errorReturn, int af, int type, int protocol, SOCKET* sock);
void __ar_make_sockaddrin(ADDRESS_FAMILY addressFamily, u_long ip, u_short port, sockaddr_in* socketAddress);
bool __ar_bind(SocketError* errorReturn, SOCKET& sock, const sockaddr * socketAddress);
bool __ar_listen(SocketError* errorReturn, SOCKET sock, int backlog);
bool __ar_accept(SocketError* errorReturn, SOCKET serverSocket, SOCKET* clientSocket, sockaddr* clientAddress);
bool __ar_connect(SocketError* errorReturn, SOCKET sock, const sockaddr* socketAddress);
