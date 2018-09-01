#include "stdafx.h"
#include "scn_login.h"

#include "CmdTokener.h"
inline void SetNextScene_Lobby(SOCKET mySocket, SocketReciver* reciver);



scn_login::scn_login(SOCKET mySocket, SocketReciver* reciver)
	: m_mySocket(mySocket)
	, m_reciver(reciver)

	, m_waitRecive(false)
	, m_endOfScene(false)
{
}
scn_login::~scn_login()
{
}



void scn_login::UpdateLoop()
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

void scn_login::Update_Recive()
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
			if (msg == "Register")		Action_Register(iJSON);
			else if (msg == "Login")	Action_Login(iJSON);
			else if (msg == "Quit")		Action_Quit();
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

void scn_login::Update_Send()
{
	char buffer[1 << 16] = { NULL };
	cin.getline(buffer, 1 << 16);

	if (!strlen(buffer))
		return;

	if (buffer[0] == '/')
	{
		CmdTokener cmdTokener(buffer);
		std::string rootCmd = cmdTokener.NextToken();
		if (rootCmd == "/register")		Assay_Register(cmdTokener);
		else if (rootCmd == "/login")	Assay_Login(cmdTokener);
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



void scn_login::Assay_Register(CmdTokener& cmdTokener)
{
	std::string id, password;
	if ((id = cmdTokener.NextToken()).size() && (password = cmdTokener.NextToken()).size())
	{
		arJSON sJsonRoot;
		sJsonRoot["Message"] = "Register";
		arJSON sJson_Register;
		sJson_Register["ID"] = id;
		sJson_Register["Password"] = password;
		sJsonRoot["ID"] = sJson_Register;
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

void scn_login::Assay_Login(CmdTokener& cmdTokener)
{
	std::string id, password;
	if ((id = cmdTokener.NextToken()).size() && (password = cmdTokener.NextToken()).size())
	{
		arJSON sJsonRoot;
		sJsonRoot["Message"] = "Login";
		arJSON sJson_Login;
		sJson_Login["ID"] = id;
		sJson_Login["Password"] = password;
		sJsonRoot["ID"] = sJson_Login;
		__ar_send(m_mySocket, sJsonRoot);

		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : Login called." << endl;
		}
		m_waitRecive = true;
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Need more data" << endl;
	}
}
void scn_login::Assay_Quit()
{
	arJSON sJsonRoot;
	sJsonRoot["Message"] = "Quit";
	__ar_send(m_mySocket, sJsonRoot);

	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Quit called." << endl;
	}
	m_waitRecive = true;
}



void scn_login::Action_Register(const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("CallResult"))
	{
		if (jsonRoot["CallResult"].Int())
		{ RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Login - Register : Success\n"; }
		else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Register : Failed\n"; }
	}
	else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Register : unknwon return\n"; }
	m_waitRecive = false;
}

void scn_login::Action_Login(const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("CallResult"))
	{
		if (jsonRoot["CallResult"].Int())
		{
			{ RegionLocker locker(g_coutMutex);	cout << "Note >> scn_Login - Login : Success\n"; }
			SetNextScene_Lobby(m_mySocket, m_reciver);
			m_endOfScene = true;
		}
		else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Login : Failed\n"; }
	}
	else
		{ RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Login : unknwon return\n"; }
	m_waitRecive = false;
}

void scn_login::Action_Quit()
{
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Quit Success." << endl;
	}
	m_endOfScene = true;
	m_waitRecive = false;
}



#include "SceneManager.h"
#include "scn_lobby.h"
inline void SetNextScene_Lobby(SOCKET mySocket, SocketReciver* reciver)
{
	SingletonInst(SceneManager)->NextScene(new scn_lobby(mySocket, reciver));
}
