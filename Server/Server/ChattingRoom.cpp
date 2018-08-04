#include "stdafx.h"
#include "ChattingRoom.h"

#include <iostream>
using std::cout;
using std::endl;
#include <string>
#include <functional>




void ChatRoomParticipant::RecvLoop(ChatRoom * chatRoom, ChatRoomParticipant * userData)
{
	User& user = userData->m_user;
	while (true)
	{
		char buffer[1 << 16] = { NULL };
		int recvResult = recv(user.socket, buffer, sizeof(buffer), NULL);
		if (recvResult > 0)
		{
			buffer[recvResult] = NULL;
			cout << "Log : Message : " << buffer << endl;
			if (std::string("-q") == buffer ||
				std::string("-Q") == buffer)
			{
				send(user.socket, "-q", 2, NULL);
			}
			else
				chatRoom->Broadcast(*userData, buffer, recvResult);
		}
		else
		{
			std::string broadcastMessage = inet_ntoa(user.address.sin_addr) + ':' + std::to_string(ntohs(user.address.sin_port)) + "´ÔÀÌ ¹æÀ» ³ª°¡¼Ì½À´Ï´Ù.";
			chatRoom->Broadcast(*userData, broadcastMessage.data(), broadcastMessage.size());

			closesocket(user.socket);

			chatRoom->Quit(*userData);
			break;
		}
	}
}




ChatRoom::ChatRoom()
{
}
ChatRoom::~ChatRoom()
{
}



bool ChatRoom::Join(User newUser)
{
	m_userListMutex.lock();
	m_userList.push_back(ChatRoomParticipant());
	m_userListMutex.unlock();

	ChatRoomParticipant& participant = *(--m_userList.end());
	participant.m_user = newUser;
	participant.m_recvLoop = std::thread(ChatRoomParticipant::RecvLoop, this, &participant);

	g_coutMutex.lock();
	cout << "Client connected : " << inet_ntoa(participant.m_user.address.sin_addr) << ':' << ntohs(participant.m_user.address.sin_port) << endl;
	g_coutMutex.unlock();
	return false;
}

void ChatRoom::Quit(const ChatRoomParticipant & participant)
{
	m_userListMutex.lock();
	for (auto iter = m_userList.begin(); iter != m_userList.end(); iter++)
	{
		if (*iter == participant)
		{
			(*iter).m_recvLoop.detach();
			m_userList.erase(iter);
			break;
		}
	}
	m_userListMutex.unlock();

	g_coutMutex.lock();
	cout << "Client disconnected : " << inet_ntoa(participant.m_user.address.sin_addr) << ':' << ntohs(participant.m_user.address.sin_port) << endl;
	g_coutMutex.unlock();
}

void ChatRoom::Broadcast(const ChatRoomParticipant& broadcaster, const char * message, int msgLength)
{
	m_userListMutex.lock();
	for (auto& participant : m_userList)
	{
		if (participant == broadcaster)
			continue;

		send(participant.m_user.socket, message, msgLength, NULL);
	}
	m_userListMutex.unlock();
}
