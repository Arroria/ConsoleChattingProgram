#pragma once
class UM_Lobby
{
public:
	static bool Reciver(AsyncConnector& client, int recvResult, SocketBuffer& socketBuffer);

private:
	static bool Act_Logout(AsyncConnector& user, const arJSON& jsonRoot);
	static bool Act_CreateRoom(AsyncConnector& user, const arJSON& jsonRoot);
	static bool Act_EnterRoom(AsyncConnector& user, const arJSON& jsonRoot);
	static bool Act_Quit(AsyncConnector& user);
};
