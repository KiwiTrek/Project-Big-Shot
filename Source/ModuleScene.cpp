#include "ModuleScene.h"
#include "Application.h"

#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "ModuleGameObjects.h"
#include "ModuleRenderer3D.h"

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

	root = new GameObject("Main");
	App->gameObjects->selectedGameObject = root;

	//Create main camera
	App->gameObjects->mainCamera = new GameObject("Camera");
	App->gameObjects->mainCamera->CreateComponent(ComponentTypes::CAMERA);
	App->gameObjects->mainCamera->GetComponent<Transform>()->SetPos(0.0f, 2.0f, 0.0f);
	App->gameObjects->mainCamera->GetComponent<Camera>()->culling = true;
	App->gameObjects->mainCamera->GetComponent<Camera>()->mainCamera = true;

	App->gameObjects->AddGameobject(App->gameObjects->mainCamera);

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
	if (drawMouse) App->renderer3D->DrawRay();
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleScene::Load(std::string scene)
{
	char* buffer = nullptr;
	if (App->fileSystem->Load(scene.c_str(), &buffer))
	{
		rapidjson::Document document;
		if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
		{
			LOG_CONSOLE("ERROR: scene  not loaded.");
		}
		else
		{
			const rapidjson::Value& scene = document.GetObjectJSON();
			App->gameObjects->selectedGameObject = nullptr;
			App->gameObjects->gameObjectList.clear();
			GameObject* newRoot = new GameObject("Main");
			root = newRoot;
			if (scene.HasMember("Game Objects"))
			{
				const rapidjson::Value& goArray = scene["Game Objects"];
				for (rapidjson::Value::ConstValueIterator it = goArray.Begin(); it != goArray.End(); ++it)
				{
					const jsonObject& ob = it->GetObjectJSON();
					root->OnLoad(ob, App);
				}
			}
			LOG_CONSOLE("Scene loaded correctly.");
		}
	}
	RELEASE_ARRAY(buffer);
}

void ModuleScene::Save(std::string scene)
{
	rapidjson::StringBuffer sb;
	JSONWriter writer(sb);

	writer.StartObject();
	writer.String("Game Objects");
	writer.StartArray();
	root->OnSave(writer);
	writer.EndArray();
	writer.EndObject();

	if (App->fileSystem->Save(scene.c_str(), sb.GetString(), strlen(sb.GetString()), false))
	{
		LOG_CONSOLE("Scene saved correctly.");
	}
	else
	{
		LOG_CONSOLE("ERROR: scene not saved.");
	}
}