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

bool ChatRoomMessenger::Send(const SocketBuffer& sockBuff)
{
	__ar_send(m_serverSocket, sockBuff, NULL);
	return false;
}

bool ChatRoomMessenger::DisconnectRoom()
{
	m_recvLoop.detach();
	SocketBuffer temp;
	temp.DataLength(2);
	char* tempBuff = temp.Buffer();
	tempBuff[0] = '-';
	tempBuff[1] = 'q';
	Send(temp);
	return false;
}

void ChatRoomMessenger::DrawMessageList()
{
	gotoxy(0, 11);
	m_msgListMutex.lock();
	for (auto& message : m_messageList)
		cout << message << endl;
	m_msgListMutex.unlock();
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
			{
				_this->m_msgListMutex.lock();
				_this->m_messageList.push_back(buffer);
				while (_this->m_messageList.size() > 10)
					_this->m_messageList.pop_front();
				_this->m_msgListMutex.unlock();
			}
		}
		else
			break;
	}
}
