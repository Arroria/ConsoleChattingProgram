#include "stdafx.h"
#include "SceneManager.h"


SceneManager::SceneManager()
	: m_currScene(nullptr)
	, m_nextScene(nullptr)
{
}
SceneManager::~SceneManager()
{
	if (m_currScene)
	{
		delete m_currScene;
		m_currScene = nullptr;
	}
	if (m_nextScene)
	{
		delete m_nextScene;
		m_nextScene = nullptr;
	}
}



bool SceneManager::Run()
{
	if (m_currScene)
	{
		delete m_currScene;
		m_currScene = nullptr;
	}
	if (!m_nextScene)
		return true;

	m_currScene = m_nextScene;
	m_nextScene = nullptr;
	m_currScene->UpdateLoop();

	return false;
}

void SceneManager::NextScene(Scene * nextScene)
{
	m_nextScene = nextScene;
}
