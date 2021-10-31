#include "Globals.h"
#include "Application.h"

#include "ModuleGameObjects.h"
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
	App->gameObjects->selectedGameObject = root;

	GameObject* g = new GameObject("Grid");
	g->CreatePrimitive(MeshTypes::Primitive_Grid);
	g->SetAxis(true);
	App->gameObjects->AddGameobject(g);

	return ret;
}

// Load assets
bool ModuleScene::Start()
{
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
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}