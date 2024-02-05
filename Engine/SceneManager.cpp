#include "pch.h"
#include "SceneManager.h"

void SceneManager::Update()
{
	if (_currentScene == nullptr)
		return;

	_currentScene->Update();
	_currentScene->LateUpdate();
	_currentScene->FinalUpdate();

}

void SceneManager::Render()
{
	if (_currentScene)
		_currentScene->Render();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}

void SceneManager::SetDefaultLayerMask()
{
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
}