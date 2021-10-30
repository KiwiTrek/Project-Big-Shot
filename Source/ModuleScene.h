#ifndef __MODULE_SCENE_INTRO_H__
#define __MODULE_SCENE_INTRO_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

#define BOUNCER_TIME 200

class PyramidP;
class CubeP;
class Mesh;
class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Init();
	bool Start();
	update_status Update(float dt);
	update_status PostUpdate();

	GameObject* GetSceneRoot() { return root; }
	bool CleanUp();

public:
	GameObject* selectedGameObject = nullptr;

	//TESTING PURPOSES
	GameObject* c;
	GameObject* pyd;
	float rotation;
private:
	GameObject* root;
};

#endif // !__MODULE_SCENE_INTRO_H__