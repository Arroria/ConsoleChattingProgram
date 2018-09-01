#include "stdafx.h"
#include "UM_Room.h"
#include "UM_Lobby.h"



bool UM_Room::Reciver(AsyncConnector & client, int recvResult, SocketBuffer & socketBuffer)
{
	bool _return = false;
	auto ReciveRoopStop = [&_return]() { _return = true; };

	if (recvResult > 0)
	{
		arJSON iJSON;
		JSON_To_arJSON(socketBuffer.Buffer(), iJSON);

		std::string& msg = iJSON["Message"].Str();
			 if (msg == "ChatMsg")	{ if (Act_Chatting(client, iJSON))	; }
		else if (msg == "Quit")		{ if (Act_Quit(client))				client.Returner(UM_Lobby::Reciver); }
		else { RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Login : Unknown data recived\n"; }
	}
	else
	{
		RegionLocker locker(g_coutMutex);
		cout << "scn_room : recv return error\n";
		ReciveRoopStop();
	}
	return _return;
}

bool UM_Room::Act_Chatting(AsyncConnector & user, const arJSON & jsonRoot)
{
	std::string message;
	if (jsonRoot.IsIn("ChatMsg"))
		message = jsonRoot["ChatMsg"].Str();
	else
	{
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Chatting : parameter error\n"; }
		return false;
	}

	std::list<SOCKET>* list = nullptr;
	if (!SingletonInst(TestRoomDB)->RoomSocketList(user, list))
	{
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Chatting : ?\n"; }
		return false;
	}

	arJSON oJSON;
	oJSON["Message"] = "ChatMsg";
	oJSON["ChatMsg"] = message;
	for (auto iter : *list)
	{
		if (iter != user.Socket())
			__ar_send(iter, oJSON);
	}
	return false;
}

bool UM_Room::Act_Quit(AsyncConnector & user)
{
	arJSON oJSON;
	oJSON["Message"] = "Quit";
	oJSON["CallResult"] = 1;
	__ar_send(user.Socket(), oJSON);
	return false;
}



bool TestRoomDB::CreateRoom(const std::string & name, const std::string & password)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	if (m_roomPassword.find(name) == m_roomPassword.end())
	{
		m_roomPassword[name] = password;
		m_room_socket[name];
		return true;
	}
	return false;
}

bool TestRoomDB::DeleteRoom(const std::string & name)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	if (m_roomPassword.find(name) != m_roomPassword.end())
	{
		m_roomPassword.erase(name);
		m_room_socket.erase(name);
		for (auto iter = m_socket_room.begin(); iter != m_socket_room.end(); )
		{
			if (iter->second == name)
				iter = m_socket_room.erase(iter);
			else
				iter++;
		}
		return true;
	}
	return false;
}

bool TestRoomDB::JoinRoom(const std::string & name, const std::string & password, SOCKET client)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	if (m_roomPassword.find(name) != m_roomPassword.end() && m_roomPassword[name] == password)
	{
		m_room_socket[name].push_back(client);
		m_socket_room.insert(std::make_pair(client, name));
		return true;
	}
	return false;
}

bool TestRoomDB::QuitRoom(const std::string & name, SOCKET client)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	if (m_roomPassword.find(name) != m_roomPassword.end())
	{
		auto& socketList = m_room_socket[name];
		for (auto iter = socketList.begin(); iter != socketList.end(); )
		{
			if (*iter == client)
				iter = socketList.erase(iter);
			else
				iter++;
		}
		m_socket_room.erase(client);
		return true;
	}
	return false;
}


bool TestRoomDB::RoomSocketList(SOCKET client, OUT std::list<SOCKET>*& list)
{
	std::lock_guard<std::mutex> locker(m_mutex);
	if (m_socket_room.find(client) != m_socket_room.end())
	{
		list = &m_room_socket[m_socket_room[client]];
		return true;
	}
	return false;
}
