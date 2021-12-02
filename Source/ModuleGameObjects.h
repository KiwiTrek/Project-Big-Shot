#ifndef __MODULE_GAME_OBJECTS_H__
#define __MODULE_GAME_OBJECTS_H__

#include "Module.h"
#include "GameObject.h"
#include <vector>

class ModuleGameObjects : public Module
{
public:
	ModuleGameObjects(Application* app, bool startEnabled = true);
	~ModuleGameObjects();

	bool Init();
	bool Start();

	UpdateStatus Update(float dt);
	UpdateStatus UpdateChildren(GameObject* parent);
	UpdateStatus PostUpdate();

	bool CleanUp();

	void AddGameobject(GameObject* g);
	void RemoveGameobject(GameObject* g);

public:
	std::vector<GameObject*> gameObjectList;
	GameObject* selectedGameObject = nullptr;
	GameObject* mainCamera = nullptr;

private:
	void RenderChildren(GameObject* parent);
};

#endif // !__MODULE_GUI_MANAGER_H__