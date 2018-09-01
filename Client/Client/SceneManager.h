#pragma once
#include "Singleton.h"
#include "scene.h"

class SceneManager :
	public Singleton<SceneManager>
{
public:
	SceneManager();
	~SceneManager();

public:
	bool Run();
	void NextScene(Scene* nextScene);

private:
	Scene* m_currScene;
	Scene* m_nextScene;
};

