#include "stdafx.h"
#include "scn_room.h"


scn_room::scn_room(SOCKET mySocket, SocketReciver * reciver)
	: m_mySocket(mySocket)
	, m_reciver(reciver)

	, m_waitRecive(false)
	, m_endOfScene(false)
{
}
scn_room::~scn_room()
{
}



void scn_room::UpdateLoop()
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

void scn_room::Update_Recive()
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
			if (msg == "ChatMsg")		Action_Chatting(iJSON);
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

void scn_room::Update_Send()
{
	char buffer[1 << 16] = { NULL };
	cin.getline(buffer, 1 << 16);

	if (!strlen(buffer))
		return;

	if (buffer[0] == '/')
	{
		CmdTokener cmdTokener(buffer);
		std::string rootCmd = cmdTokener.NextToken();
		if (rootCmd == "/exit")	Assay_Quit();
		else
		{
			std::lock_guard<std::mutex> locker(g_coutMutex);
			cout << "Error : Is not command" << endl;
		}
	}
	else
	{
		if (strlen(buffer) > 0)
			Assay_Chatting(buffer);
	}
}



void scn_room::Assay_Chatting(const std::string& message)
{
	arJSON sJsonRoot;
	sJsonRoot["Message"] = "ChatMsg";
	sJsonRoot["ChatMsg"] = message;
	__ar_send(m_mySocket, sJsonRoot);
}

void scn_room::Assay_Quit()
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



void scn_room::Action_Chatting(const arJSON & jsonRoot)
{
	if (jsonRoot.IsIn("ChatMsg"))
	{
		const std::string& message = jsonRoot["ChatMsg"].Str();
		{
			RegionLocker locker(g_coutMutex);
			cout << "Message : " << message << endl;
		}
	}
	else
	{
		RegionLocker locker(g_coutMutex);	cout << "Error >> scn_Login - Login : unknwon return\n";
	}
	m_waitRecive = false;
}

void scn_room::Action_Quit()
{
	{
		std::lock_guard<std::mutex> locker(g_coutMutex);
		cout << "Note : Quit Success." << endl;
	}
	m_endOfScene = true;
	m_waitRecive = false;
}
