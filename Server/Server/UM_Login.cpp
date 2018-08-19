#include "stdafx.h"
#include "UM_Login.h"


UM_Login::UM_Login()
{
}
UM_Login::~UM_Login()
{
	for (auto& thread : m_user)
		thread.second.detach();
	m_user.clear();
}



bool UM_Login::Join(const User & user)
{
	std::lock_guard<std::mutex> locker(m_userMutex);
	if (m_user.find(user) == m_user.end())
	{
		m_user.insert(std::make_pair(user, std::thread(RecvLoop, user)));
		return false;
	}
	return true;
}

bool UM_Login::Quit(const User & user)
{
	std::lock_guard<std::mutex> locker(m_userMutex);
	auto userIter = m_user.find(user);
	if (userIter != m_user.end())
	{
		if (userIter->second.joinable())
			userIter->second.detach();
		m_user.erase(userIter);
		return false;
	}
	return true;
}



void Action_Register(const User& user, const arJSON& jsonRoot);
void Action_Login(const User& user, const arJSON& jsonRoot);
void Action_Quit(const User& user);
void UM_Login::RecvLoop(User user)
{
	while (true)
	{
		SocketBuffer socBuf;
		int recvResult = __ar_recv(user.socket, socBuf);
		if (recvResult > 0)
		{
			socBuf[recvResult] = NULL;
			arJSON jsonRoot;
			JSON_To_arJSON(socBuf.Buffer(), jsonRoot);

			if (jsonRoot.IsIn("Message"))
			{
				const std::string& msg = jsonRoot["Message"].Str();

				constexpr char msg_register[] = "Register";
				constexpr char msg_login[] = "Login";
				constexpr char msg_quit[] = "Quit";

				if		(msg == msg_register)	Action_Register(user, jsonRoot);
				else if (msg == msg_login)		Action_Login(user, jsonRoot);
				else if (msg == msg_quit)		Action_Quit(user);
				else
				{
					std::lock_guard<std::mutex> locker(g_coutMutex);
					cout << "UM_Login " << inet_ntoa(user.address.sin_addr) << ':' << std::to_string(user.address.sin_port) << " : Unknown message getted : " << msg << endl;
				}
			}
			else
			{
				std::lock_guard<std::mutex> locker(g_coutMutex);
				cout << "UM_Login " << inet_ntoa(user.address.sin_addr) << ':' << std::to_string(user.address.sin_port) << " : Message no have" << endl;
			}
		}
		else
		{
			{
				std::lock_guard<std::mutex> locker(g_coutMutex);
				cout << "UM_Login " << inet_ntoa(user.address.sin_addr) << ':' << std::to_string(user.address.sin_port) << " : recv return <= 0" << endl;
			}
			SingletonInst(UM_Login)->Quit(user);
		}
	}
}



void SendData(const User & user, const arJSON & jsonRoot);
void Action_Register(const User & user, const arJSON & jsonRoot)
{
	bool registIDSucess = false;
	if (jsonRoot.IsIn("ID"))
	{
		const arJSON& json_ID = jsonRoot["ID"].Sub();
		if (json_ID.IsIn("ID") &&
			json_ID.IsIn("Password"))
		{
			const std::string& id = json_ID["ID"].Str();
			const std::string& password = json_ID["Password"].Str();

			if (!SingletonInst(TestIDDB)->RegistID(id, password))
				registIDSucess = true;
		}
	}


	arJSONValue jsonV_Register;
	registIDSucess ?
		jsonV_Register = 0 :
		jsonV_Register = 1;
	arJSON sendJson;
	sendJson["Message"] = "Register";
	sendJson["Register"] = jsonV_Register;
	
	SendData(user, sendJson);
}

void Action_Login(const User & user, const arJSON & jsonRoot)
{
	bool loginSucess = false;
	if (jsonRoot.IsIn("ID"))
	{
		const arJSON& json_ID = jsonRoot["ID"].Sub();
		if (json_ID.IsIn("ID") &&
			json_ID.IsIn("Password"))
		{
			const std::string& id = json_ID["ID"].Str();
			const std::string& password = json_ID["Password"].Str();

			if (!SingletonInst(TestIDDB)->Login(id, password))
				loginSucess = true;
		}
	}


	arJSONValue jsonV_Login;
	loginSucess ?
		jsonV_Login = 0 :
		jsonV_Login = 1;
	arJSON sendJson;
	sendJson["Message"] = "Login";
	sendJson["Login"] = jsonV_Login;

	/*
	추가 코드 필요 : 로그인
	*/

	SendData(user, sendJson);
}

void Action_Quit(const User & user)
{
	arJSON sendJson;
	sendJson["Message"] = "Quit";
	sendJson["Quit"] = 0;
	SendData(user, sendJson);

	SingletonInst(UM_Login)->Quit(user);
}



void SendData(const User & user, const arJSON & jsonRoot)
{
	SocketBuffer socBuf;
	SetSocketBuffer(jsonRoot.ToJSON(), socBuf);
	__ar_send(user.socket, socBuf);
}







bool TestIDDB::RegistID(const std::string & id, const std::string & password)
{
	if (m_id.find(id) == m_id.end())
	{
		m_id.insert(std::make_pair(id, password));
		return false;
	}
	return true;
}

bool TestIDDB::Login(const std::string & id, const std::string & password)
{
	auto idIter = m_id.find(id);
	if (idIter != m_id.end())
		return idIter->second != password;
	return true;
}
