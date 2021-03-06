#pragma once
#include "scene.h"
#include "CmdTokener.h"

class scn_login :
	public Scene
{
public:
	scn_login(SOCKET mySocket, SocketReciver* reciver);
	~scn_login();

public:
	virtual void UpdateLoop();

private:
	void Update_Recive();
	void Update_Send();


	void Assay_Register(CmdTokener& cmdTokener);
	void Assay_Login(CmdTokener& cmdTokener);
	void Assay_Quit();

	void Action_Register(const arJSON& jsonRoot);
	void Action_Login(const arJSON& jsonRoot);
	void Action_Quit();

private:
	SOCKET m_mySocket;
	SocketReciver* m_reciver;

	bool m_waitRecive;
	bool m_endOfScene;
};
