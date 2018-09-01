#include "stdafx.h"
#include "UM_Lobby.h"
#include "UM_Login.h"
#include "UM_Room.h"
bool Room_Name_Password(const arJSON & jsonRoot, OUT std::string & name, OUT std::string & password);



bool UM_Lobby::Reciver(AsyncConnector & client, int recvResult, SocketBuffer & socketBuffer)
{
	bool _return = false;
	auto ReciveRoopStop = [&_return]() { _return = true; };

	if (recvResult > 0)
	{
		arJSON iJSON;
		JSON_To_arJSON(socketBuffer.Buffer(), iJSON);

		std::string& msg = iJSON["Message"].Str();
			 if (msg == "Logout")		{ if (Act_Logout(client, iJSON))		client.Returner(UM_Login::Reciver); }
		else if (msg == "CreateRoom")	{ if (Act_CreateRoom(client, iJSON))	; }
		else if (msg == "EnterRoom")	{ if (Act_EnterRoom(client, iJSON))		client.Returner(UM_Room::Reciver); }
		else if (msg == "Quit")			{ if (Act_Quit(client))					ReciveRoopStop(); }
		else { RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Lobby : Unknown data recived\n"; }
	}
	else
	{
		RegionLocker locker(g_coutMutex);
		cout << "scn_Lobby : recv return error\n";
		ReciveRoopStop();
	}
	return _return;
}



bool UM_Lobby::Act_Logout(AsyncConnector & user, const arJSON & jsonRoot)
{
	arJSON oJSON;
	oJSON["Message"] = "Logout";
	oJSON["CallResult"] = 1;
	__ar_send(user.Socket(), oJSON);

	{ RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Lobby >> Logout : Success\n"; }
	return true;
}

bool UM_Lobby::Act_CreateRoom(AsyncConnector & user, const arJSON & jsonRoot)
{
	int result = 0;

	std::string name, password;
	if (Room_Name_Password(jsonRoot, name, password))
	{
		if (SingletonInst(TestRoomDB)->CreateRoom(name, password))
		{ 
			{ RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Room - CreateRoom : id added - " << name << endl; }
			result = 1;
		}
		else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Room - CreateRoom : name overlapped - " << name << endl; }
	}
	else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Room - CreateRoom : parameter error\n"; }

	arJSON oJSON;
	oJSON["Message"] = "CreateRoom";
	oJSON["CallResult"] = result;
	__ar_send(user.Socket(), oJSON);
	return false;
}

bool UM_Lobby::Act_EnterRoom(AsyncConnector & user, const arJSON & jsonRoot)
{
	int result = 0;

	std::string name, password;
	if (Room_Name_Password(jsonRoot, name, password))
	{
		if (SingletonInst(TestRoomDB)->JoinRoom(name, password, user.Socket()))
		{ 
			{ RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Room - JoinRoom : Join success - " << name << endl; }
			result = 1;
		}
		else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Room - JoinRoom : Join failed - " << name << endl; }
	}
	else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Room - JoinRoom : parameter error\n"; }

	arJSON oJSON;
	oJSON["Message"] = "EnterRoom";
	oJSON["CallResult"] = result;
	__ar_send(user.Socket(), oJSON);
	return result;
}

bool UM_Lobby::Act_Quit(AsyncConnector & user)
{
	arJSON oJSON;
	oJSON["Message"] = "Quit";
	oJSON["CallResult"] = 1;
	__ar_send(user.Socket(), oJSON);
	return false;
}



bool Room_Name_Password(const arJSON & jsonRoot, OUT std::string & name, OUT std::string & password)
{
	if (jsonRoot.IsIn("Room"))
	{
		const arJSON& json_ID = jsonRoot["Room"].Sub();
		if (json_ID.IsIn("Name") && json_ID.IsIn("Password"))
		{
			name = json_ID["Name"].Str();
			password = json_ID["Password"].Str();
			return true;
		}
	}
	return false;
}
