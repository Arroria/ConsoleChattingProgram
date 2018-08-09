#include "stdafx.h"
#include "ChattingServer.h"

#include "ChattingRoom.h"

ChattingServer::ChattingServer()
{
	m_roomList.resize(ChatRoomListSize, nullptr);
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
			for (size_t index = 0; index < ChatRoomListSize; index++)
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
