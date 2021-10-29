#include "Globals.h"
#include "Application.h"
#include "Gameobject.h"
#include "ModuleSceneIntro.h"

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
	Gameobject* g = new Gameobject("Grid");
	g->CreatePrimitive(MeshTypes::Primitive_Grid);
	g->SetAxis(true);
	App->importer->AddGameobject(g);

	c = new Gameobject("Cube");
	c->CreatePrimitive(MeshTypes::Primitive_Cube);
	c->GetTransform()->SetPos(-2, 0, -3);
	App->importer->AddGameobject(c);

	pyd = new Gameobject("Pyramid");
	pyd->CreatePrimitive(MeshTypes::Primitive_Pyramid);
	pyd->GetTransform()->SetPos(2, 0, -3);
	App->importer->AddGameobject(pyd);

	//Gameobject* s = new Gameobject("Sphere");
	//s->CreatePrimitive(MeshTypes::Primitive_Sphere);
	//s->GetTransform()->SetPos(0, 0, -3);
	//App->importer->AddGameobject(s);

	//Gameobject* cyl = new Gameobject("Cyinder");
	//cyl->CreatePrimitive(MeshTypes::Primitive_Cylinder);
	//cyl->GetTransform()->SetPos(0, 0, -6);
	//App->importer->AddGameobject(cyl);
	
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
	//c->GetTransform()->SetRot(0, rotation, 0);
	//pyd->GetTransform()->SetRot(0, rotation, 0);

	return update_status::UPDATE_CONTINUE;
}
update_status ModuleSceneIntro::PostUpdate()
{
	App->renderer3D->Render();
	return update_status::UPDATE_CONTINUE;
}