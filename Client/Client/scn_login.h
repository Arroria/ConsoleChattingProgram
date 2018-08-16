#pragma once
#include "scene.h"
#include <mutex>
#include <thread>

class scn_login :
	public Scene
{
public:
	scn_login();
	~scn_login();

public:
	arJSON UpdateLoop(SOCKET mySocket);

private:
	static void RecvLoop(scn_login& scene, SOCKET mySocket);

	void Send_Register(const std::string& id, const std::string& password);
	void Send_Login(const std::string& id, const std::string& password);
	void Send_Quit();

	void Action_Register();
	void Action_Login();
	void Action_Quit();

	inline void ReturnGetted()
	{
		m_returnGetMutex.lock();
		m_returnGet = true;
		m_returnGetMutex.unlock();
	}
	inline void WaitReturnGet()
	{
		while (true)
		{
			m_returnGetMutex.lock();
			if (m_returnGet)
			{
				m_returnGet = false;
				m_returnGetMutex.unlock();
				break;
			}
			else
				m_returnGetMutex.unlock();
		}
	}
	inline void SceneExpired()
	{
		m_scnExpiredMutex.lock();
		m_sceneExpired = true;
		m_scnExpiredMutex.unlock();
	}
	inline bool IsSceneExpired()
	{
		m_scnExpiredMutex.lock();
		if (m_sceneExpired)
		{
			m_scnExpiredMutex.unlock();
			return true;
		}
		else
			m_scnExpiredMutex.unlock();
		return false;
	}

private:
	SOCKET m_mySocket;

	std::thread m_recvLoop;

	bool m_sceneExpired;
	std::mutex m_scnExpiredMutex;
	bool m_returnGet;
	std::mutex m_returnGetMutex;

	arJSON m_resultJSON;
};

