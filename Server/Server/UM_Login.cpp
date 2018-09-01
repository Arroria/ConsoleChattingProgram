#include "stdafx.h"
#include "UM_Login.h"
#include "UM_Lobby.h"
bool ID_Password(const arJSON & jsonRoot, OUT std::string & id, OUT std::string & password);


bool UM_Login::Reciver(AsyncConnector & client, int recvResult, SocketBuffer & socketBuffer)
{
	bool _return = false;
	auto ReciveRoopStop = [&_return]() { _return = true; };

	if (recvResult > 0)
	{
		arJSON iJSON;
		JSON_To_arJSON(socketBuffer.Buffer(), iJSON);

		std::string& msg = iJSON["Message"].Str();
			 if (msg == "Register")	{ if (Act_Register(client, iJSON))	; }
		else if (msg == "Login")	{ if (Act_Login(client, iJSON))		client.Returner(UM_Lobby::Reciver); }
		else if (msg == "Quit")		{ if (Act_Quit(client))				ReciveRoopStop(); }
		else { RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Login : Unknown data recived\n"; }
	}
	else
	{
		RegionLocker locker(g_coutMutex);
		cout << "scn_Login : recv return error\n";
		ReciveRoopStop();
	}
	return _return;
}



bool UM_Login::Act_Register(AsyncConnector & user, const arJSON & jsonRoot)
{
	int result = 0;

	std::string id, password;
	if (ID_Password(jsonRoot, id, password))
	{
		if (SingletonInst(TestIDDB)->RegistID(id, password))
		{ 
			{ RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Login - Register : id added - " << id << endl; }
			result = 1;
		}
		else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Register : id overlapped - " << id << endl; }
	}
	else
	{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Register : parameter error\n"; }

	arJSON oJSON;
	oJSON["Message"] = "Register";
	oJSON["CallResult"] = result;
	__ar_send(user.Socket(), oJSON);
	return false;
}

bool UM_Login::Act_Login(AsyncConnector & user, const arJSON & jsonRoot)
{
	int result = 0;

	std::string id, password;
	if (ID_Password(jsonRoot, id, password))
	{
		if (SingletonInst(TestIDDB)->Login(id, password))
		{
			{ RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Login - Login : Login approved - " << id << endl; }
			result = 1;
		}
		else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Login : Login disapproved" << id << endl; }
	}
	else
	{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Login : parameter error\n"; }

	arJSON oJSON;
	oJSON["Message"] = "Login";
	oJSON["CallResult"] = result;
	__ar_send(user.Socket(), oJSON);
	return result;
}

bool UM_Login::Act_Quit(AsyncConnector & user)
{
	arJSON oJSON;
	oJSON["Message"] = "Quit";
	oJSON["CallResult"] = 1;
	__ar_send(user.Socket(), oJSON);
	return false;
}



bool ID_Password(const arJSON & jsonRoot, OUT std::string & id, OUT std::string & password)
{
	if (jsonRoot.IsIn("ID"))
	{
		const arJSON& json_ID = jsonRoot["ID"].Sub();
		if (json_ID.IsIn("ID") && json_ID.IsIn("Password"))
		{
			id = json_ID["ID"].Str();
			password = json_ID["Password"].Str();
			return true;
		}
	}
	return false;
}










bool TestIDDB::RegistID(const std::string & id, const std::string & password)
{
	if (m_id.find(id) == m_id.end())
	{
		m_id.insert(std::make_pair(id, password));
		return true;
	}
	return false;
}

bool TestIDDB::Login(const std::string & id, const std::string & password)
{
	auto idIter = m_id.find(id);
	if (idIter != m_id.end())
		return idIter->second == password;
	return false;
}



