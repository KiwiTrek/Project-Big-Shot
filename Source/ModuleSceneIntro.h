#ifndef __MODULE_SCENE_INTRO_H__
#define __MODULE_SCENE_INTRO_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

#define BOUNCER_TIME 200

class PyramidP;
class CubeP;
class Mesh;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Init();
	bool Start();
	update_status Update(float dt);
	update_status PostUpdate();
	bool CleanUp();

public:
	CubeP* c;
	PyramidP* pyd;
	float rotation;
};

#endif // !__MODULE_SCENE_INTRO_H__