#pragma once
#include <list>
#include <thread>
#include <mutex>
#include "../../arSocket.h"

struct User
{
	SOCKET socket;
	sockaddr_in address;

	bool operator==(const User& ant)
	{
		return 
			address.sin_port == ant.address.sin_port &&
			memcmp(&address.sin_addr, &ant.address.sin_addr, sizeof(in_addr));
	}
};

class ChattingRoom
{
public:
	ChattingRoom();
	~ChattingRoom();


public:
	bool Join(User newUser);

private:
	void Broadcast(std::list<User>::iterator broadcaster, const char* message, int msgLength);

	void RecvLoop(std::list<User>::iterator userIter, std::list<std::thread>::iterator threadIter);
	static void stt_RecvLoop(ChattingRoom* _this, void* userIter, void* threadIter) { _this->RecvLoop(*(std::list<User>::iterator*)userIter, *(std::list<std::thread>::iterator*)threadIter); }

private:
	std::list<User> m_userList;
	std::list<std::thread> m_thread;

	std::mutex m_userListMutex;
	std::mutex m_threadMutex;
};

