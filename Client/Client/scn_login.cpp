#include "stdafx.h"
#include "scn_login.h"

#include "UserCommand.h"

scn_login::scn_login()
	: m_mySocket(NULL)

	, m_sceneExpired(false)
	, m_returnGet(false)
{
}
scn_login::~scn_login()
{
}



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
				UserCommand cmd(buffer);

				std::string rootCmd = cmd.NextToken();
				if (rootCmd == "/register")
				{
					std::string id, password;
					if ((id = cmd.NextToken()).size() && (password = cmd.NextToken()).size())
					{
						Send_Register(id, password);
						WaitReturnGet();
						cout << "Register called." << endl;
					}
					else
						cout << "Need more data" << endl;
				}
				else if (rootCmd == "/login")
				{
					std::string id, password;
					if ((id = cmd.NextToken()).size() && (password = cmd.NextToken()).size())
					{
						Send_Login(id, password);
						WaitReturnGet();
					}
					else
						cout << "Need more data" << endl;
				}
				else if (rootCmd == "/exit")
				{
					Send_Quit();
					WaitReturnGet();
				}
				else
					cout << "Is not command" << endl;
			}
			else
			{
				//Nothing
			}
		}
	}


	m_recvLoop.detach();
	return m_resultJSON;
}



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
				std::string msg = jsonRoot["Message"].Str();
				if (msg == "RegisterSuccess")
				{
					scene.Action_Register();
					scene.ReturnGetted();
				}
				else if (msg == "LoginSuccess")
				{
					scene.Action_Login();
					scene.ReturnGetted();
				}
				else if (msg == "RegisterFailed")
				{
					cout << "Register Failed" << endl;
					scene.ReturnGetted();
				}
				else if (msg == "LoginFailed")
				{
					cout << "Login Failed" << endl;
					scene.ReturnGetted();
				}
				else if (msg == "QuitSuccess")
				{
					scene.Action_Quit();
					scene.ReturnGetted();
				}
			}
			else
				cout << "Recived useless message" << endl;
		}
		else
			scene.Action_Quit();
	}
}



void scn_login::Send_Register(const std::string& id, const std::string& password)
{
	std::string jsonStr;
	{
		arJSON jsonRoot;
		jsonRoot["Message"] = "Register";
		jsonRoot["ID"] = id;
		jsonRoot["Password"] = password;

		jsonRoot.ToJSON(jsonStr);
	}

	SocketBuffer sockBuff;
	SetSocketBuffer(jsonStr, sockBuff);
	__ar_send(m_mySocket, sockBuff);
}

void scn_login::Send_Login(const std::string& id, const std::string& password)
{
	std::string jsonStr;
	{
		arJSON jsonRoot;
		jsonRoot["Message"] = "Login";
		arJSON jsonID;
		jsonRoot["ID"] = id;
		jsonRoot["Password"] = password;

		jsonRoot.ToJSON(jsonStr);
	}

	SocketBuffer sockBuff;
	SetSocketBuffer(jsonStr, sockBuff);
	__ar_send(m_mySocket, sockBuff);
}

void scn_login::Send_Quit()
{
	std::string jsonStr;
	{
		arJSON jsonRoot;
		jsonRoot["Message"] = "Quit";
		jsonRoot.ToJSON(jsonStr);
	}

	SocketBuffer sockBuff;
	SetSocketBuffer(jsonStr, sockBuff);
	__ar_send(m_mySocket, sockBuff);
}



void scn_login::Action_Register()
{
	cout << "Register Success" << endl;
}

void scn_login::Action_Login()
{
	cout << "Login Success" << endl;
	m_resultJSON["Message"] = "Login";
	SceneExpired();
}

void scn_login::Action_Quit()
{
	cout << "Quit" << endl;
	m_resultJSON["Message"] = "Quit";
	SceneExpired();
}
