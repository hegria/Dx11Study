#pragma once
#include "Scene.h"

enum
{
	MAX_LAYER = 32
};

class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);
	void SetDefaultLayerMask();

	template<typename T>
	void ChangeScene(shared_ptr<T> scene)
	{
		_currentScene = scene;
		scene->Start();
	}

	shared_ptr<Scene> GetCurrentScene() { return _currentScene; }

private:
	shared_ptr<Scene> _currentScene = make_shared<Scene>();

	array<wstring, MAX_LAYER> _layerNames;
	map<wstring, uint8> _layerIndex;
};

