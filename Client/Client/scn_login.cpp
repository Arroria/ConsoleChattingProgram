#include "stdafx.h"
#include "scn_login.h"

#include "CmdTokener.h"

scn_login::scn_login()
	: m_mySocket(NULL)

	, m_sceneExpired(false)
	, m_returnGet(false)
{
}
scn_login::~scn_login()
{
}



void Assay_Register(scn_login& scene, SOCKET mySocket, CmdTokener& cmdTokener);
void Assay_Login(scn_login& scene, SOCKET mySocket, CmdTokener& cmdTokener);
void Assay_Quit(scn_login& scene, SOCKET mySocket);
arJSON scn_login::UpdateLoop(SOCKET mySocket)
{
	m_mySocket = mySocket;
	m_recvLoop = std::thread(RecvLoop, std::ref(*this), m_mySocket);



	while (true)
	{
		if (IsSceneExpired())
			break;


		char buffer[1 << 16] = { NULL };
		cin.getline(buffer, 1 << 16);

		if (strlen(buffer))
		{
			if (buffer[0] == '/')
			{
				CmdTokener cmdTokener(buffer);
				std::string rootCmd = cmdTokener.NextToken();
				if (rootCmd == "/register")		Assay_Register(*this, mySocket, cmdTokener);	
				else if (rootCmd == "/login")	Assay_Login(*this, mySocket, cmdTokener);
				else if (rootCmd == "/exit")	Assay_Quit(*this, mySocket);
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
	}


	m_recvLoop.detach();
	return m_resultJSON;
}



void SendData(SOCKET mySocket, const arJSON& jsonRoot);
void Assay_Register(scn_login& scene, SOCKET mySocket, CmdTokener& cmdTokener)
{
	std::string id, password;
	if ((id = cmdTokener.NextToken()).size() && (password = cmdTokener.NextToken()).size())
	{
		arJSON sJsonRoot;
		sJsonRoot["Message"] = "Register";
		//arJSON& sJson_Register = sJsonRoot["ID"].Sub();
		//sJson_Register["ID"] = id;
		//sJson_Register["password"] = password;
		
		arJSON sJson_Register;
		sJson_Register["ID"] = id;
		sJson_Register["Password"] = password;
		sJsonRoot["ID"] = sJson_Register;

		SendData(mySocket, sJsonRoot);

		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : Register called." << endl;
		}
		scene.WaitReturnGet();
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Need more data" << endl;
	}
}

void Assay_Login(scn_login& scene, SOCKET mySocket, CmdTokener& cmdTokener)
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

		SendData(mySocket, sJsonRoot);

		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : Login called." << endl;
		}
		scene.WaitReturnGet();
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Need more data" << endl;
	}
}
void Assay_Quit(scn_login& scene, SOCKET mySocket)
{
	arJSON sJsonRoot;
	sJsonRoot["Message"] = "Quit";

	SendData(mySocket, sJsonRoot);

	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Quit called." << endl;
	}
	scene.WaitReturnGet();
}



void SendData(SOCKET mySocket, const arJSON & jsonRoot)
{
	SocketBuffer socBuf;
	SetSocketBuffer(jsonRoot.ToJSON(), socBuf);
	__ar_send(mySocket, socBuf);
}



void Action_Register(scn_login& scene, const arJSON& jsonRoot);
void Action_Login(scn_login& scene, const arJSON& jsonRoot);
void Action_Quit(scn_login& scene);
void scn_login::RecvLoop(scn_login& scene, SOCKET mySocket)
{
	while (!scene.IsSceneExpired())
	{
		SocketBuffer sockBuff;
		int result = __ar_recv(mySocket, sockBuff);
		if (result > 0)
		{
			sockBuff[result] = NULL;
			arJSON jsonRoot;
			JSON_To_arJSON(sockBuff.Buffer(), jsonRoot);
			if (jsonRoot.IsIn("Message"))
			{
				const std::string& msg = jsonRoot["Message"].Str();
				if (msg == "Register")		Action_Register(scene, jsonRoot);
				else if (msg == "Login")	Action_Login(scene, jsonRoot);
				else if (msg == "Quit")		Action_Quit(scene);
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
			Action_Quit(scene);
		}
	}
}




void Action_Register(scn_login & scene, const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("Register"))
	{
		if (!jsonRoot["Register"].Int())
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Note : Register Success." << endl;
		}
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : Register Failed." << endl;
		}
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Register unknown error." << endl;
	}
	scene.ReturnGetted();
}

void Action_Login(scn_login & scene, const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("Login"))
	{
		if (!jsonRoot["Login"].Int())
		{
			/*
			로그인 아웃플 필요
			*/
			{
				std::lock_guard<std::mutex> locker(g_coutMutex);
				cout << "Note : Login Success." << endl;
			}
		}
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : Login Failed." << endl;
		}
	}
	else
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Error : Login unknown error." << endl;
	}
	scene.ReturnGetted();
}

void Action_Quit(scn_login & scene)
{
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Quit Success." << endl;
	}
	scene.ReturnGetted();
	/*
	종료 처리 필요
	*/
}

