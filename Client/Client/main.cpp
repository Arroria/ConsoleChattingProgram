#include "stdafx.h"

#include "ChatRoomMessenger.h"

SOCKET g_socket;
sockaddr_in g_serverAddress;

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

	__ar_make_sockaddrin(AF_INET, htonl(INADDR_LOOPBACK), htons(8888), &g_serverAddress);

	if (__ar_connect(&socErr, g_socket, (sockaddr*)&g_serverAddress))
		ErrorReturn(socErr, "connect()");



	SocketBuffer sockBuff;
	int result = NULL;
	if ((result = __ar_recv(g_socket, sockBuff)) <= 0)
		cout << "error" << endl;
	else
	{
		sockBuff[result] = NULL;
		arJSON jsonRoot;
		JSON_To_arJSON(sockBuff.Buffer(), jsonRoot);

		for (auto room : jsonRoot["RoomList"])
			cout << "Room : " << room["id"].Int() << endl;
	}
	


	/*
	cout << "Room Number : ";
	ChatRoomMessenger crmng;
	crmng.ConnectRoom(g_socket, g_serverAddress);
	while (true)
	{
		SocketBuffer buffer;
		cin.getline(buffer.Buffer(), buffer.bufferSize);
		if (std::string("-q") == buffer.Buffer())
		{
			crmng.DisconnectRoom();
			break;
		}
		
		crmng.Send(buffer, strlen(buffer));
		system("cls");
		crmng.DrawMessageList();
		gotoxy(0, 11);
	}
	*/



	cout << "sleep" << endl;
	Sleep(5000);

	closesocket(g_socket);
	WSACleanup();
	system("pause");
}