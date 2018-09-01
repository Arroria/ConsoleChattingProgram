#pragma once
#include "scene.h"
#include "CmdTokener.h"

class scn_lobby :
	public Scene
{
public:
	scn_lobby(SOCKET mySocket, SocketReciver* reciver);
	~scn_lobby();

public:
	virtual void UpdateLoop();

private:
	void Update_Recive();
	void Update_Send();


	void Assay_Logout(CmdTokener& cmdTokener);
	void Assay_CreateRoom(CmdTokener& cmdTokener);
	void Assay_EnterRoom(CmdTokener& cmdTokener);
	void Assay_Quit();

	void Action_Logout(const arJSON& jsonRoot);
	void Action_CreateRoom(const arJSON& jsonRoot);
	void Action_EnterRoom(const arJSON& jsonRoot);
	void Action_Quit();

private:
	SOCKET m_mySocket;
	SocketReciver* m_reciver;

	bool m_waitRecive;
	bool m_endOfScene;
};
