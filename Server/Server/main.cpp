#include "stdafx.h"

#include "ChattingServer.h"
#include "ChattingRoom.h"
void TempThread(SOCKET clientSocket);


#include "UM_Login.h"


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



	ChattingServer chatServer;
	chatServer.CreateRoom(1);
	chatServer.CreateRoom(5);
	chatServer.CreateRoom(3);
	chatServer.CreateRoom(2);
	chatServer.CreateRoom(4);

	chatServer.CreateRoom(63);
	chatServer.CreateRoom(52);
	chatServer.CreateRoom(57);
	chatServer.CreateRoom(36);
	chatServer.CreateRoom(67);



	while (true)
	{
		User user;
		SOCKET& clientSocket = user.socket;
		sockaddr_in& clientAddress = user.address;
		if (__ar_accept(&socErr, g_socket, &clientSocket, (sockaddr*)&clientAddress))
			ErrorReturn(socErr, "listen()");

		SingletonInst(UM_Login)->Join(user);
	}


	closesocket(g_socket);
	WSACleanup();
	system("pause");
}
