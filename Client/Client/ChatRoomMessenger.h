#pragma once
#include <thread>
#include "../../arSocket.h"


class ChatRoomMessenger
{
public:
	ChatRoomMessenger();
	~ChatRoomMessenger();

public:
	bool ConnectRoom(SOCKET serverSocket, const sockaddr_in& serverAddress);
	bool Send(const char* message, int msgLength);
	bool DisconnectRoom();

private:
	static void RecvLoop(ChatRoomMessenger* _this);

private:
	SOCKET m_serverSocket;
	sockaddr_in m_serverAddress;

	std::thread m_recvLoop;
};

