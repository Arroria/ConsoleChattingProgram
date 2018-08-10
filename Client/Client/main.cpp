#include "stdafx.h"
#include "../../arSocket.h"

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

	__ar_make_sockaddrin(AF_INET, inet_addr("222.108.204.37"), htons(8888), &g_serverAddress);

	if (__ar_connect(&socErr, g_socket, (sockaddr*)&g_serverAddress))
		ErrorReturn(socErr, "connect()");



	SocketBuffer sockBuff;
	int result = NULL;
	if ((result = __ar_recv(g_socket, sockBuff)) <= 0)
		cout << "error" << endl;
	else
	{
		if (!strncmp(sockBuff.Buffer(), "-roomlist", 9))
		{
			using RoomNumber_t = int32_t;

			int roomCnt = (result - 10) / sizeof(RoomNumber_t);
			RoomNumber_t* currBuff = (RoomNumber_t*)(&sockBuff[10]);
			for (size_t count = 0; count < roomCnt; count++)
			{
				cout << ntohl(*currBuff) << endl;
				currBuff++;
			}
		}
		else
			cout << "roomlist 이거 없대" << endl;
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