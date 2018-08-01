#include "stdafx.h"
#include "../../arSocket.h"


SOCKET g_socket;
sockaddr_in g_socketAddress;

void ErrorReturn(const SocketError& socErr, const char* errorCaller)
{
	cout << errorCaller << " Error : " << socErr.errorName << "(" << socErr.errorCode << ')' << endl;
	system("pause");
	exit(1);
}

void main()
{
	SocketError socErr;

	WSAData wsaData;
	if (__ar_WSAStartup(&socErr, 0x0202, &wsaData))
		ErrorReturn(socErr, "WSAStartup()");
		
	if (__ar_socket(&socErr, AF_INET, SOCK_STREAM, NULL, &g_socket))
		ErrorReturn(socErr, "socket()");

	__ar_make_sockaddrin(AF_INET, htonl(INADDR_ANY), htons(8888), &g_socketAddress);

	if (__ar_bind(&socErr, g_socket, (sockaddr*)&g_socketAddress))
		ErrorReturn(socErr, "bind()");

	if (__ar_listen(&socErr, g_socket, 64))
		ErrorReturn(socErr, "listen()");



	SOCKET clientSocket;
	sockaddr_in clientAddress;
	if (__ar_accept(&socErr, g_socket, &clientSocket, (sockaddr*)&clientAddress))
		ErrorReturn(socErr, "listen()");

	char message[] = "Hello, world!";
	send(clientSocket, message, strlen(message), NULL);



	closesocket(g_socket);
	WSACleanup();
	system("pause");
}