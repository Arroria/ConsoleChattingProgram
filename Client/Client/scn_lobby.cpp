#include "stdafx.h"
#include "scn_lobby.h"

#include "CmdTokener.h"
inline void SetNextScene_Login(SOCKET mySocket, SocketReciver* reciver);
inline void SetNextScene_Room(SOCKET mySocket, SocketReciver* reciver);



scn_lobby::scn_lobby(SOCKET mySocket, SocketReciver* reciver)
	: m_mySocket(mySocket)
	, m_reciver(reciver)

	, m_waitRecive(false)
	, m_endOfScene(false)
{
}
scn_lobby::~scn_lobby()
{
}



void scn_lobby::UpdateLoop()
{
	while (!m_endOfScene)
	{
		if (m_reciver->IsRecived())
		{
			Update_Recive();
			m_reciver->ReciveRestart();
		}
		else
		{
			if (!m_waitRecive)
			{
				Update_Send();
			}
		}
	}
}

void scn_lobby::Update_Recive()
{
	std::lock_guard<SocketReciver> locker(*m_reciver);
	int recvResult = m_reciver->ReciveResult();
	if (recvResult > 0)
	{
		arJSON iJSON;
		{
			SocketBuffer& data = m_reciver->RecivedData();
			data[recvResult] = NULL;
			iJSON = data.Buffer();
		}
		
		if (iJSON.IsIn("Message"))
		{
			const std::string& msg = iJSON["Message"].Str();
			if (msg == "Logout")			Action_Logout(iJSON);
			else if (msg == "EnterRoom")	Action_EnterRoom(iJSON);
			else if (msg == "CreateRoom")	Action_CreateRoom(iJSON);
			else if (msg == "Quit")			Action_Quit();
			else
			{
				std::lock_guard<std::mutex> locker(g_coutMutex);
				cout << "Error : Unknown message recieved." << endl;
			}
		}
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : Message do not recieved." << endl;
		}
	}
	else
	{
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : Server disconnected." << endl;
		}
		Action_Quit();
	}
}

void scn_lobby::Update_Send()
{
	char buffer[1 << 16] = { NULL };
	cin.getline(buffer, 1 << 16);

	if (!strlen(buffer))
		return;

	if (buffer[0] == '/')
	{
		CmdTokener cmdTokener(buffer);
		std::string rootCmd = cmdTokener.NextToken();
		if (rootCmd == "/logout")		Assay_Logout(cmdTokener);
		else if (rootCmd == "/join")	Assay_EnterRoom(cmdTokener);
		else if (rootCmd == "/create")	Assay_CreateRoom(cmdTokener);
		else if (rootCmd == "/exit")	Assay_Quit();
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : Is not command" << endl;
		}
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Only command activate." << endl;
	}
}



void scn_lobby::Assay_Logout(CmdTokener & cmdTokener)
{
	arJSON sJsonRoot;
	sJsonRoot["Message"] = "Logout";
	__ar_send(m_mySocket, sJsonRoot);

	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Logout called." << endl;
	}
	m_waitRecive = true;
}

void scn_lobby::Assay_CreateRoom(CmdTokener & cmdTokener)
{
	std::string name, password;
	if ((name = cmdTokener.NextToken()).size())
	{
		password = cmdTokener.NextToken();

		arJSON sJsonRoot;
		sJsonRoot["Message"] = "CreateRoom";
		arJSON sJson_Register;
		sJson_Register["Name"] = name;
		sJson_Register["Password"] = password.size() ?
			password : "";
		sJsonRoot["Room"] = sJson_Register;
		__ar_send(m_mySocket, sJsonRoot);

		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : Register called." << endl;
		}
		m_waitRecive = true;
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Need more data" << endl;
	}
}

void scn_lobby::Assay_EnterRoom(CmdTokener & cmdTokener)
{
	std::string name, password;
	if ((name = cmdTokener.NextToken()).size())
	{
		password = cmdTokener.NextToken();

		arJSON sJsonRoot;
		sJsonRoot["Message"] = "EnterRoom";
		arJSON sJson_Register;
		sJson_Register["Name"] = name;
		sJson_Register["Password"] = password.size() ?
			password : "";
		sJsonRoot["Room"] = sJson_Register;
		__ar_send(m_mySocket, sJsonRoot);

		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : Register called." << endl;
		}
		m_waitRecive = true;
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Need more data" << endl;
	}
}

void scn_lobby::Assay_Quit()
{
	arJSON sJsonRoot;
	sJsonRoot["Message"] = "Quit";
	__ar_send(m_mySocket, sJsonRoot);

	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Logout called." << endl;
	}
	m_waitRecive = true;
}



void scn_lobby::Action_Logout(const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("CallResult"))
	{
		if (jsonRoot["CallResult"].Int())
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : Logout Success." << endl;
			SetNextScene_Login(m_mySocket, m_reciver);
			m_endOfScene = true;
		}
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : Logout Failed." << endl;
		}
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Logout unknown error." << endl;
	}
	m_waitRecive = false;
}

void scn_lobby::Action_CreateRoom(const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("CallResult"))
	{
		if (jsonRoot["CallResult"].Int())
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : CreateRoom Success." << endl;
		}
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : CreateRoom Failed." << endl;
		}
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : CreateRoom unknown error." << endl;
	}
	m_waitRecive = false;
}

void scn_lobby::Action_EnterRoom(const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("CallResult"))
	{
		if (jsonRoot["CallResult"].Int())
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : EnterRoom Success." << endl;
			SetNextScene_Room(m_mySocket, m_reciver);
			m_endOfScene = true;
		}
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : EnterRoom Failed." << endl;
		}
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : EnterRoom unknown error." << endl;
	}
	m_waitRecive = false;
}

void scn_lobby::Action_Quit()
{
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Quit Success." << endl;
	}
	m_endOfScene = true;
}



#include "SceneManager.h"
#include "scn_login.h"
#include "scn_room.h"
inline void SetNextScene_Login(SOCKET mySocket, SocketReciver* reciver)
{
	SingletonInst(SceneManager)->NextScene(new scn_login(mySocket, reciver));
}
inline void SetNextScene_Room(SOCKET mySocket, SocketReciver* reciver)
{
	SingletonInst(SceneManager)->NextScene(new scn_room(mySocket, reciver));
}

