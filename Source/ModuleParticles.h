#ifndef __MODULE_PARTICLES_H__
#define __MODULE_PARTICLES_H__

#define MAX_PARTICLES 20000

#include "Module.h"
#include "Globals.h"
#include <vector>

class GameObject;
class ResourceMesh;

class ModuleParticles : public Module
{
public:
	ModuleParticles(Application* app, bool startEnabled = true);
	~ModuleParticles();

	bool Init();
	bool Start();
	UpdateStatus Update(float dt);
	UpdateStatus PostUpdate();
	bool CleanUp();


	/*void Load(std::string scene);
	void Save(std::string scene);*/

public:
	std::vector<GameObject*> emitters;

	ResourceMesh* plane = nullptr;

	int activeParticles = 0;
	GameObject* firework = nullptr;
};

#endif // !__MODULE_PARTICLES_H__