#include "stdafx.h"

#include "ChatRoomMessenger.h"

#include "SceneManager.h"
#include "scn_login.h"


void ErrorReturn(const SocketError& socErr, const char* errorCaller)
{
	cout << errorCaller << " Error : " << socErr.errorName << "(" << socErr.errorCode << ')' << endl;
	system("pause");
	exit(1);
}

void main()
{
	SocketError socErr;

	SOCKET serverSocket;
	sockaddr_in serverAddress;

	WSAData wsaData;
	if (__ar_WSAStartup(&socErr, 0x0202, &wsaData))
		ErrorReturn(socErr, "WSAStartup()");

	if (__ar_socket(&socErr, AF_INET, SOCK_STREAM, NULL, &serverSocket))
		ErrorReturn(socErr, "socket()");

	__ar_make_sockaddrin(AF_INET, htonl(INADDR_LOOPBACK), htons(8888), &serverAddress);

	if (__ar_connect(&socErr, serverSocket, (sockaddr*)&serverAddress))
		ErrorReturn(socErr, "connect()");



	SocketReciver* socketRevicer = new SocketReciver(serverSocket, serverAddress);
	socketRevicer->Run();

	SingletonInst(SceneManager)->NextScene(new scn_login(serverSocket, socketRevicer));
	while (!SingletonInst(SceneManager)->Run());



	cout << "sleep" << endl;
	Sleep(5000);

	closesocket(serverSocket);
	WSACleanup();
	system("pause");
}