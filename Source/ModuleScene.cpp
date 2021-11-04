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

	grid = new GameObject("Grid");
	grid->CreatePrimitive(MeshTypes::Primitive_Grid);
	grid->SetAxis(true);

	return ret;
}

// Load assets
bool ModuleScene::Start()
{
	return true;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	root->children.clear();
	delete root;
	root = nullptr;

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