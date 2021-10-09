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

	PlaneP* p = new PlaneP(0, 1, 0, 0);
	p->axis = true;
	App->renderer3D->AddPrimitive(p);

	c = new CubeP(1, 1, 1);
	c->SetPos(-2, 0, 0);
	App->renderer3D->AddPrimitive(c);

	pyd = new PyramidP(2, 2, 2);
	pyd->SetPos(2, 0, 0);
	App->renderer3D->AddPrimitive(pyd);

	SphereP* s = new SphereP();
	App->renderer3D->AddPrimitive(s);

	rotation = 0.0f;
	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Unloading Intro scene"));

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update(float dt)
{
	rotation += 0.5f;
	if (rotation >= 360.0f)
	{
		rotation = 0.0f;
	}
	pyd->SetRotation(rotation, vec3(0, 1, 0));
	c->SetRotation(rotation, vec3(0, 1, 0));
	return UPDATE_CONTINUE;
}
update_status ModuleSceneIntro::PostUpdate()
{
	App->renderer3D->Render();
	return UPDATE_CONTINUE;
}