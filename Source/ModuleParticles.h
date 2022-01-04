#ifndef __MODULE_PARTICLES_H__
#define __MODULE_PARTICLES_H__

#include "Module.h"
#include "Globals.h"
#include <vector>
#include "Particle.h"

class GameObject;
class ResourceMesh;
class EmitterData;

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

	GameObject* CreateEmitter(EmitterData dat);
	void DeleteEmitter(GameObject* e);

	/*void Load(std::string scene);
	void Save(std::string scene);*/

public:
	std::vector<GameObject*> emitters;

	ResourceMesh* plane = nullptr;
	int activeParticles = 0;
	GameObject* firework = nullptr;
};

#endif // !__MODULE_PARTICLES_H__