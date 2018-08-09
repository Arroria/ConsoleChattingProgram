#pragma once
#include <mutex>
#include <vector>

class ChatRoom;
class ChattingServer
{
	static constexpr size_t ChatRoomListSize = 1000;

public:
	ChattingServer();
	~ChattingServer();

	ChatRoom& operator[](size_t index) { return *m_roomList[index]; }


public:
	int CreateRoom(int roomID = -1);
	void DestroyRoom(int roomID);


private:
	std::vector<ChatRoom*> m_roomList;
	std::mutex m_roomListMutex;
};

