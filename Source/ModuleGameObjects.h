#ifndef __MODULE_GAME_OBJECTS_H__
#define __MODULE_GAME_OBJECTS_H__

#include "Module.h"
#include "GameObject.h"
#include "Globals.h"
#include <vector>

class ModuleGameObjects : public Module
{
public:
	ModuleGameObjects(Application* app, bool start_enabled = true);
	~ModuleGameObjects();

	bool Init();
	bool Start();
	update_status PostUpdate();
	bool CleanUp();

	void AddGameobject(GameObject* g);
	void RemoveGameobject(GameObject* g);

public:
	std::vector<GameObject*> gameobjectList;
	GameObject* selectedGameObject = nullptr;

private:
	void RenderChildren(GameObject* parent);
};

#endif // !__MODULE_GUI_MANAGER_H__