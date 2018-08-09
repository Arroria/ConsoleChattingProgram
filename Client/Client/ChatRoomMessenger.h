#pragma once
#include <thread>
#include <mutex>
#include "../../arSocket.h"

#include <list>
#include <string>

class ChatRoomMessenger
{
public:
	ChatRoomMessenger();
	~ChatRoomMessenger();

public:
	bool ConnectRoom(SOCKET serverSocket, const sockaddr_in& serverAddress);
	bool Send(const SocketBuffer& sockBuff);
	bool DisconnectRoom();

	void DrawMessageList();

private:
	static void RecvLoop(ChatRoomMessenger* _this);

private:
	SOCKET m_serverSocket;
	sockaddr_in m_serverAddress;

	std::thread m_recvLoop;

	std::list<std::string> m_messageList;
	std::mutex m_msgListMutex;
};

