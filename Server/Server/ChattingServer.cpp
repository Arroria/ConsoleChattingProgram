#include "stdafx.h"
#include "ChattingServer.h"

#include "ChattingRoom.h"


ChattingServer::ChattingServer()
{
	m_roomList.resize(chatRoomListSize, nullptr);
}
ChattingServer::~ChattingServer()
{
}



int ChattingServer::CreateRoom(int roomID)
{
	int result = NULL;
	m_roomListMutex.lock();
	{
		auto _CreateRoom = [](ChatRoom** roomPlace)
		{
			*roomPlace = new ChatRoom();
		};

		if (roomID >= 0)
		{
			if (!m_roomList[roomID])
			{
				_CreateRoom(&m_roomList[roomID]);
				result = roomID;
			}
			else
				result = NULL;
		}
		else
		{
			int roomIndex = -1;
			for (size_t index = 0; index < chatRoomListSize; index++)
			{
				if (!m_roomList[index])
				{
					roomIndex = index;
					break;
				}
			}
			_CreateRoom(&m_roomList[roomIndex]);
			result = roomIndex;
		}
	}
	m_roomListMutex.unlock();
	return result;
}

void ChattingServer::DestroyRoom(int roomID)
{
	m_roomListMutex.lock();
	ChatRoom* room = m_roomList[roomID];
	if (room)
	{
		//room->Release();
		delete room;
		room = nullptr;
	}
	m_roomListMutex.unlock();
}



#include "../../arSocket.h"
#include <string>
bool ChattingServer::SendRoomList(User & user)
{
	using RoomNumber_t = ChatRoom::RoomNumber_t;


	arJSON json;
	arJSONValue& roomList = json["Room"];
	
	m_roomListMutex.lock();
	for (RoomNumber_t number = 0; number < chatRoomListSize; number++)
	{
		ChatRoom* room = m_roomList[number];
		if (!room)
			continue;

		arJSON jsonRoom;
		jsonRoom["id"] = number;
		roomList.Append(jsonRoom);
	}
	m_roomListMutex.unlock();



	std::string jsonStr;
	json.ToJSON(jsonStr);

	SocketBuffer sockBuff;
	strcpy(sockBuff.Buffer(), jsonStr.data());
	sockBuff.DataLength(jsonStr.size());
	const char* temp = sockBuff.Buffer();
	__ar_send(user.socket, sockBuff);
	return false;
}
