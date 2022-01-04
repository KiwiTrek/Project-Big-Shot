#include "ModuleScene.h"
#include "Application.h"

#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "ModuleGameObjects.h"
#include "ModuleRenderer3D.h"
#include "ModuleParticles.h"

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
	EmitterData smoke;

	smoke.eType = EmitterData::EmitterType::SMOKE;
	smoke.sType = EmitterData::EmitterSphere::RANDOM;

	smoke.duration = 1.0f;
	smoke.loop = true;

	smoke.burst = false;
	smoke.minPart = 0;
	smoke.maxPart = 10;
	smoke.repeatTime = 1.0f;

	smoke.gravity = 0.0f;
	smoke.particleDirection = float3::unitY;

	smoke.cubeCreation = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));
	smoke.sphereCreation = Sphere(float3::zero, 1.0f);
	smoke.circleCreation = Circle(float3::unitY, float3::unitY, 0.25f);
	smoke.shapeType = Shape::SPHERE;

	smoke.sizeOBB = float3::zero;
	smoke.drawAABB = false;
	smoke.posDifAABB = float3::zero;

	smoke.plane = App->particles->plane;
	smoke.texture = (ResourceMaterial*)App->resources->RequestResource("smoke.png");
	smoke.textureRows = 1;
	smoke.textureColumns = 1;

	smoke.checkLife = true;
	smoke.checkSpeed = true;
	smoke.checkAcceleration = true;
	smoke.checkSize = true;
	smoke.checkSizeOverTime = true;
	smoke.checkRotation = false;
	smoke.checkAngularAcceleration = true;
	smoke.checkAngularVelocity = true;

	smoke.isSubEmitter = false;
	smoke.subEmitter = nullptr;
	smoke.subEmitterUUID = 0u;

	smoke.rateOverTime = 15;
	smoke.timeToParticle = 0.0f;
	smoke.subEmitterActive = false;

	smoke.particleLife = float2(5.0f, 10.0f);
	smoke.speed = float2(3.0f, 3.0f);
	smoke.acceleration = float2(0.0f, 0.0f);
	smoke.sizeOverTime = float2(0.0f, 0.0f);
	smoke.size = float2(1.0f, 1.0f);
	smoke.rotation = float2(0.0f, 0.0f);
	smoke.angularAcceleration = float2(0.0f, 0.0f);
	smoke.angularVelocity = float2(0.0f, 0.0f);

	smoke.color.push_back(FadeColor(Color(1.0f,1.0f,1.0f,1.0f), 0.0f, true));
	smoke.color.push_back(FadeColor(Color(1.0f,1.0f,1.0f,0.0f), 1.0f, true));
	smoke.timeColor = true;

	App->gameObjects->AddGameobject(App->particles->CreateEmitter(smoke));

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
	if (drawMouse) App->renderer3D->DrawRay(ray);
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