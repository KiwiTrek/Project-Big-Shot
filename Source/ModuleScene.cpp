#include "Globals.h"
#include "Application.h"
#include "Gameobject.h"
#include "ModuleScene.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "scene";
}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Init()
{
	LOG_CONSOLE("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1, 2, 1));
	App->camera->LookAt(vec3(0, 0, 0));

	root = new GameObject("sceneRoot");
	selectedGameObject = root;

	GameObject* g = new GameObject("Grid");
	g->CreatePrimitive(MeshTypes::Primitive_Grid);
	g->SetAxis(true);
	App->gameObjects->AddGameobject(g);

	rotation = 0.0f;
	return ret;
}

// Load assets
bool ModuleScene::Start()
{
	c = new GameObject("Cube");
	c->CreatePrimitive(MeshTypes::Primitive_Cube);
	c->GetTransform()->SetPos(-2, 0, -3);
	App->gameObjects->AddGameobject(c);

	pyd = new GameObject("Pyramid");
	pyd->CreatePrimitive(MeshTypes::Primitive_Pyramid);
	pyd->GetTransform()->SetPos(2, 0, -3);
	App->gameObjects->AddGameobject(pyd);

	GameObject* s = new GameObject("Sphere");
	s->CreatePrimitive(MeshTypes::Primitive_Sphere);
	s->GetTransform()->SetPos(0, 0, -3);
	App->gameObjects->AddGameobject(s);

	GameObject* cyl = new GameObject("Cyinder");
	cyl->CreatePrimitive(MeshTypes::Primitive_Cylinder);
	cyl->GetTransform()->SetPos(0, 0, -6);
	App->gameObjects->AddGameobject(cyl);
	
	return true;
}

bool ModuleScene::CleanUp()
{
	LOG_CONSOLE("Unloading Intro scene");

	return true;
}

// Update: draw background
update_status ModuleScene::Update(float dt)
{
	rotation += 0.5f;
	if (rotation >= 360.0f)
	{
		rotation = 0.0f;
	}
	c->GetTransform()->SetRot(0, rotation, 0);
	pyd->GetTransform()->SetRot(0, rotation, 0);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}