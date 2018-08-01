#include "stdafx.h"
#include "ChattingRoom.h"

#include <iostream>
using std::cout;
using std::endl;
#include <string>
#include <functional>

ChattingRoom::ChattingRoom()
{
}
ChattingRoom::~ChattingRoom()
{
}



bool ChattingRoom::Join(User newUser)
{
	m_userList.push_back(newUser);
	auto& userIter = --m_thread.end();
	m_thread.push_back(std::thread());
	auto& threadIter = --m_thread.end();
	*threadIter = std::thread(stt_RecvLoop, this, &userIter, &threadIter);

	return false;
}

void ChattingRoom::Broadcast(std::list<User>::iterator broadcaster, const char * message, int msgLength)
{
	m_userListMutex.lock();
	for (auto userIter = m_userList.begin(); userIter != m_userList.end(); userIter++)
	{
		if (userIter == broadcaster)
			continue;

		send(userIter->socket, message, msgLength, NULL);
	}
	m_userListMutex.unlock();
}



void ChattingRoom::RecvLoop(std::list<User>::iterator userIter, std::list<std::thread>::iterator threadIter)
{
	User& user = *userIter;
	while (true)
	{
		char buffer[1 << 16] = { NULL };
		int recvResult = recv(user.socket, buffer, sizeof(buffer), NULL);
		if (recv > 0)
		{
			buffer[recvResult] = NULL;
			Broadcast(userIter, buffer, recvResult);
		}
		else
		{
			std::string broadcastMessage = inet_ntoa(user.address.sin_addr) + ':' + std::to_string(ntohs(user.address.sin_port)) + "¥‘¿Ã πÊ¿ª ≥™∞°ºÃΩ¿¥œ¥Ÿ.";
			Broadcast(userIter, broadcastMessage.data(), broadcastMessage.size());
			
			closesocket(user.socket);

			g_coutMutex.lock();
			if (!recvResult)
				cout << "Client disconnected : " << inet_ntoa(user.address.sin_addr) << ':' << ntohs(user.address.sin_port) << endl;
			else
				cout << "Client disconnected by error : " << inet_ntoa(user.address.sin_addr) << ':' << ntohs(user.address.sin_port) << endl;
			g_coutMutex.unlock();

			m_userListMutex.lock();
			m_threadMutex.lock();
			m_userList.erase(userIter);
			m_thread.erase(threadIter);
			m_threadMutex.unlock();
			m_userListMutex.unlock();
			break;
		}
	}
}
