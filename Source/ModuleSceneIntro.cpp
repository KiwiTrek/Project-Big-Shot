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

bool ModuleSceneIntro::Init()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Loading Intro assets"));
	bool ret = true;
	App->camera->Move(vec3(0, 2, 0));
	App->camera->LookAt(vec3(0, 0, 0));

	rotation = 0.0f;
	return ret;
}

// Load assets
bool ModuleSceneIntro::Start()
{
	Grid* g = new Grid(0, 1, 0, 0);
	g->axis = true;
	App->importer->AddPrimitive(g);

	c = new CubeP();
	c->SetPos(-2, 0, -3);
	App->importer->AddPrimitive(c);

	pyd = new PyramidP();
	pyd->SetPos(2, 0, -3);
	App->importer->AddPrimitive(pyd);

	SphereP* s = new SphereP();
	s->SetPos(0, 0, -3);
	App->importer->AddPrimitive(s);

	CylinderP* cyl = new CylinderP();
	cyl->SetPos(0, 0, -6);
	App->importer->AddPrimitive(cyl);
	
	return true;
}

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

	return update_status::UPDATE_CONTINUE;
}
update_status ModuleSceneIntro::PostUpdate()
{
	App->renderer3D->Render();
	return update_status::UPDATE_CONTINUE;
}