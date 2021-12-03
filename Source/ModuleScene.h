#ifndef __MODULE_SCENE_INTRO_H__
#define __MODULE_SCENE_INTRO_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

#define BOUNCER_TIME 200

class GameObject;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool startEnabled = true);
	~ModuleScene();

	bool Init();
	bool Start();
	UpdateStatus Update(float dt);
	UpdateStatus PostUpdate();

	GameObject* GetSceneRoot() { return root; }
	bool CleanUp();
	void Load(std::string scene);
	void Save(std::string scene);

private:
	GameObject* root;
};

#endif // !__MODULE_SCENE_INTRO_H__