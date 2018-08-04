#include "stdafx.h"
#include "ChatRoomMessenger.h"

ChatRoomMessenger::ChatRoomMessenger()
{
}
ChatRoomMessenger::~ChatRoomMessenger()
{
}



bool ChatRoomMessenger::ConnectRoom(SOCKET serverSocket, const sockaddr_in & serverAddress)
{
	m_serverSocket = serverSocket;
	m_serverAddress = serverAddress;
	m_recvLoop = std::thread(RecvLoop, this);
	return false;
}

bool ChatRoomMessenger::Send(const char * message, int msgLength)
{
	send(m_serverSocket, message, msgLength, NULL);
	return false;
}

bool ChatRoomMessenger::DisconnectRoom()
{
	m_recvLoop.detach();
	Send("-q", 2);
	return false;
}

void ChatRoomMessenger::RecvLoop(ChatRoomMessenger* _this)
{
	char buffer[1 << 16] = { NULL };
	while (true)
	{
		int recvResult = recv(_this->m_serverSocket, buffer, sizeof(buffer), NULL);
		if (recvResult > 0)
		{
			buffer[recvResult] = NULL;
			if (std::string("-q") == buffer)
				break;
			else
				cout << buffer << endl;
		}
		else
			break;
	}
}
