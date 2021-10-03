#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "sceneIntro";
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Loading Intro assets"));
	bool ret = true;
	App->camera->Move(vec3(0, 2, 0));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Unloading Intro scene"));

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	PlaneP p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	return UPDATE_CONTINUE;
}

