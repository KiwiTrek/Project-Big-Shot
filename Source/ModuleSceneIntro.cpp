#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Mesh.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "sceneIntro";
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Init()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Loading Intro assets"));
	bool ret = true;
	App->camera->Move(vec3(0, 2, 0));
	App->camera->LookAt(vec3(0, 0, 0));

	PlaneP* p = new PlaneP(0, 1, 0, 0);
	p->axis = true;
	App->importer->AddPrimitive(p);

	c = new CubeP(1, 1, 1);
	c->SetPos(-2, 0, -3);
	App->importer->AddPrimitive(c);

	pyd = new PyramidP(2, 2, 2);
	pyd->SetPos(2, 0, -3);
	App->importer->AddPrimitive(pyd);

	SphereP* s = new SphereP();
	s->SetPos(0, 0, -3);
	App->importer->AddPrimitive(s);

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

	if (!customMeshes.empty())
	{
		std::vector<CustomMesh*>::iterator w = customMeshes.begin();
		(*w)->SetPos(0, 0, 1);
		//(*w)->SetRotation(45.0f, (1.0f, 0.0f, 0.0f)); TODO
		(*w)->Scale(0.05, 0.05, 0.05);
	}

	return update_status::UPDATE_CONTINUE;
}
update_status ModuleSceneIntro::PostUpdate()
{
	App->renderer3D->Render();
	return update_status::UPDATE_CONTINUE;
}