#pragma once
#include <mutex>
#include <vector>

#include "ServerUser.h"

class ChatRoom;
class ChattingServer
{
	static constexpr size_t chatRoomListSize = 100;

public:
	ChattingServer();
	~ChattingServer();

	ChatRoom& operator[](size_t index) { return *m_roomList[index]; }


public:
	int CreateRoom(int roomID = -1);
	void DestroyRoom(int roomID);



public:
	bool SendRoomList(User& user);


private:
	std::vector<ChatRoom*> m_roomList;
	std::mutex m_roomListMutex;
};

