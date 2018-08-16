#include "stdafx.h"

#include "ChattingServer.h"
#include "ChattingRoom.h"
void TempThread(SOCKET clientSocket);



SOCKET g_socket;
sockaddr_in g_socketAddress;

void ErrorReturn(const SocketError& socErr, const char* errorCaller)
{
	cout << errorCaller << " Error : " << socErr.errorName << "(" << socErr.errorCode << ')' << endl;
	system("pause");
	exit(1);
}

void main()
{
	SocketError socErr;

	WSAData wsaData;
	if (__ar_WSAStartup(&socErr, 0x0202, &wsaData))
		ErrorReturn(socErr, "WSAStartup()");
		
	if (__ar_socket(&socErr, AF_INET, SOCK_STREAM, NULL, &g_socket))
		ErrorReturn(socErr, "socket()");

	__ar_make_sockaddrin(AF_INET, htonl(INADDR_ANY), htons(8888), &g_socketAddress);

	if (__ar_bind(&socErr, g_socket, (sockaddr*)&g_socketAddress))
		ErrorReturn(socErr, "bind()");

	if (__ar_listen(&socErr, g_socket, 64))
		ErrorReturn(socErr, "listen()");



	ChattingServer chatServer;
	chatServer.CreateRoom(1);
	chatServer.CreateRoom(5);
	chatServer.CreateRoom(3);
	chatServer.CreateRoom(2);
	chatServer.CreateRoom(4);

	chatServer.CreateRoom(63);
	chatServer.CreateRoom(52);
	chatServer.CreateRoom(57);
	chatServer.CreateRoom(36);
	chatServer.CreateRoom(67);



	while (true)
	{
		User user;
		SOCKET& clientSocket = user.socket;
		sockaddr_in& clientAddress = user.address;
		if (__ar_accept(&socErr, g_socket, &clientSocket, (sockaddr*)&clientAddress))
			ErrorReturn(socErr, "listen()");


		std::thread(TempThread, clientSocket).detach();
		//chatServer.SendRoomList(user);
		//Sleep(1000);
	}


	closesocket(g_socket);
	WSACleanup();
	system("pause");
}

class TempDB
{
public:
	inline bool IsAlreadyIn(const std::string& id) { return m_id.find(id) != m_id.end(); }
	inline bool IdSame(const std::string& id, const std::string& password) { return m_id[id] == password; }

	inline void Register(const std::string& id, const std::string& password) { m_id[id] = password; }

private:
	std::map<std::string, std::string> m_id;
};

TempDB tpdb;
std::mutex tpdbMut;

void TempThread(SOCKET clientSocket)
{
	while (true)
	{
		SocketBuffer sockBuff;
		int result = __ar_recv(clientSocket, sockBuff);
		if (result > 0)
		{
			sockBuff[result] = NULL;
			arJSON json;
			JSON_To_arJSON(sockBuff.Buffer(), json);
			
			if (json.IsIn("Message"))
			{
				const std::string& message = json["Message"].Str();
				if (message == "Register")
				{
					bool failed = false;
					if (json.IsIn("ID") && json.IsIn("Password"))
					{
						std::string id, password;
						id = json["ID"].Str();
						password = json["Password"].Str();
						if (id.size() && password.size())
						{
							tpdbMut.lock();
							failed = tpdb.IsAlreadyIn(id);
							tpdbMut.unlock();
						}
						else
							failed = true;
					}
					else
						failed = true;


					arJSON sendJson;
					if (failed)
						sendJson["Message"] = "RegisterFailed";
					else
					{
						std::string id, password;
						id = json["ID"].Str();
						password = json["Password"].Str();

						tpdbMut.lock();
						tpdb.Register(id, password);
						tpdbMut.unlock();

						sendJson["Message"] = "RegisterSuccess";
					}


					std::string jsonStr;
					sendJson.ToJSON(jsonStr);
					SocketBuffer sockBuff;
					SetSocketBuffer(jsonStr, sockBuff);
					__ar_send(clientSocket, sockBuff);
				}
				else if (message == "Login")
				{
					bool failed = false;
					if (json.IsIn("ID") && json.IsIn("Password"))
					{
						std::string id, password;
						id = json["ID"].Str();
						password = json["Password"].Str();
						if (id.size() && password.size())
						{
							tpdbMut.lock();
							failed = !tpdb.IdSame(id, password);
							tpdbMut.unlock();
						}
						else
							failed = true;
					}
					else
						failed = true;


					arJSON sendJson;
					if (failed)
						sendJson["Message"] = "LoginFailed";
					else
						sendJson["Message"] = "LoginSuccess";


					std::string jsonStr;
					sendJson.ToJSON(jsonStr);
					SocketBuffer sockBuff;
					SetSocketBuffer(jsonStr, sockBuff);
					__ar_send(clientSocket, sockBuff);
				}
				else if (message == "Quit")
				{
					arJSON sendJson;
					sendJson["Message"] = "QuitSuccess";

					std::string jsonStr;
					sendJson.ToJSON(jsonStr);
					SocketBuffer sockBuff;
					SetSocketBuffer(jsonStr, sockBuff);
					__ar_send(clientSocket, sockBuff);
					return;
				}
			}
			else
				cout << "미정의 데이터 반환됨\n";
		}
		else
		{
			cout << "clinet discon\n";
			return;
		}
	}
}
