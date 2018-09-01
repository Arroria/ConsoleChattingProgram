#pragma once
class UM_Room
{
public:
	static bool Reciver(AsyncConnector& client, int recvResult, SocketBuffer& socketBuffer);

private:
	static bool Act_Chatting(AsyncConnector& user, const arJSON& jsonRoot);
	static bool Act_Quit(AsyncConnector& user);
};

#include <map>
#include <list>
#include <string>
#include "Singleton.h"
class TestRoomDB
	: public Singleton<TestRoomDB>
{
private:
	struct Room
	{
		std::string name;
		std::string password;
	};

public:
	bool CreateRoom(const std::string& name, const std::string& password);
	bool DeleteRoom(const std::string& name);
	bool JoinRoom(const std::string& name, const std::string& password, SOCKET client);
	bool QuitRoom(const std::string& name, SOCKET client);

	bool RoomSocketList(SOCKET client, OUT std::list<SOCKET>*& list);

private:
	std::map<std::string, std::list<SOCKET>> m_room_socket;
	std::map<SOCKET, std::string> m_socket_room;
	std::map<std::string, std::string> m_roomPassword;
	mutable std::mutex m_mutex;
};
