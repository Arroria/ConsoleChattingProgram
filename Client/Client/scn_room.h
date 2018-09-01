#pragma once
#include "scene.h"
#include "CmdTokener.h"

class scn_room :
	public Scene
{
public:
	scn_room(SOCKET mySocket, SocketReciver* reciver);
	~scn_room();

public:
	virtual void UpdateLoop();

private:
	void Update_Recive();
	void Update_Send();


	void Assay_Chatting(const std::string& message);
	void Assay_Quit();

	void Action_Chatting(const arJSON& jsonRoot);
	void Action_Quit();

private:
	SOCKET m_mySocket;
	SocketReciver* m_reciver;

	bool m_waitRecive;
	bool m_endOfScene;
};

