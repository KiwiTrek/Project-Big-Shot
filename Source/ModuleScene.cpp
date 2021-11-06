#include "Application.h"
#include "ModuleGameObjects.h"
#include "ModuleScene.h"

ModuleScene::ModuleScene(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "scene";
}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Init()
{
	bool ret = true;
	LOG_CONSOLE("Loading Scene assets");

	App->camera->Move(vec3(1, 2, 1));
	App->camera->LookAt(vec3(0, 0, 0));

	root = new GameObject("sceneRoot");
	App->gameObjects->selectedGameObject = root;

	grid = new GameObject("Grid");
	grid->CreatePrimitive(MeshTypes::Primitive_Grid);
	grid->SetAxis(true);

	return ret;
}

bool ModuleScene::Start()
{
	return true;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading scene");

	root->children.clear();
	delete root;
	root = nullptr;

	return true;
}

UpdateStatus ModuleScene::Update(float dt)
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleScene::PostUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}