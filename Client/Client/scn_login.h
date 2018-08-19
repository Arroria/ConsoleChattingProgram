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

	inline void ReturnGetted()
	{
		std::lock_guard<std::mutex> locker(m_returnGetMutex);
		m_returnGet = true;
	}
	inline void WaitReturnGet()
	{
		while (true)
		{
			std::lock_guard<std::mutex> locker(m_returnGetMutex);
			if (m_returnGet)
			{
				m_returnGet = false;
				break;
			}
		}
	}

private:
	static void RecvLoop(scn_login& scene, SOCKET mySocket);

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

