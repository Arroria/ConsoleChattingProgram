#pragma once
#include <list>
#include <thread>
#include <mutex>
#include "../../arSocket.h"

#include "ServerUser.h"



class ChatRoom;
struct ChatRoomParticipant
{
public:
	User m_user;
	std::thread m_recvLoop;

public:
	static void RecvLoop(ChatRoom* chatRoom, ChatRoomParticipant* userData);

public:
	bool operator==(const ChatRoomParticipant& ant) const { return m_user == ant.m_user; }
};


class ChattingServer;
class ChatRoom
{
public:
	ChatRoom();
	~ChatRoom();


public:
	int Join(User newUser);
	int Quit(const ChatRoomParticipant& user);
	void Broadcast(const ChatRoomParticipant& broadcaster, const char* message, int msgLength);

	//void Shutdown();






private:
	ChattingServer* m_server;

	std::list<ChatRoomParticipant> m_userList;
	std::mutex m_userListMutex;



public:
	using RoomNumber_t = int32_t;

public:
	RoomNumber_t RoomNumber() { return m_roomNumber; }

private:
	RoomNumber_t m_roomNumber;

};

