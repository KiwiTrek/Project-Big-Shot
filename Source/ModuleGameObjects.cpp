#include "ModuleGameObjects.h"
#include "Application.h"

#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"

ModuleGameObjects::ModuleGameObjects(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "game_objects";
}

ModuleGameObjects::~ModuleGameObjects()
{}

bool ModuleGameObjects::Init()
{
	return true;
}

bool ModuleGameObjects::Start()
{
	return true;
}

UpdateStatus ModuleGameObjects::Update(float dt)
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	std::vector<GameObject*>::iterator item = gameObjectList.begin();
	while (item != gameObjectList.end())
	{
		if (!(*item)->children.empty()) ret = UpdateChildren((*item));
		std::vector<Component*>::iterator c = (*item)->components.begin();
		while (c != (*item)->components.end())
		{
			(*c)->Update();
			c++;
		}
		item++;
	}
	return ret;
}

UpdateStatus ModuleGameObjects::UpdateChildren(GameObject* parent)
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	std::vector<GameObject*>::iterator item = parent->children.begin();
	while (item != parent->children.end())
	{
		if (!(*item)->children.empty()) ret = UpdateChildren((*item));
		std::vector<Component*>::iterator c = (*item)->components.begin();
		while (c != (*item)->components.end())
		{
			(*c)->Update();
			c++;
		}
		item++;
	}
	return ret;
}

UpdateStatus ModuleGameObjects::PostUpdate()
{
	std::vector<GameObject*>::iterator item = gameObjectList.begin();
	while (item != gameObjectList.end())
	{
		if (!(*item)->children.empty()) RenderChildren((*item));

		ComponentMesh* m = (*item)->GetComponent<ComponentMesh>();
		if (m != nullptr && m->IsActive())
		{
			(*item) == selectedGameObject ? m->drawBBox = true : m->drawBBox = false;
			m->wireOverride = App->renderer3D->IsWireframe();
			m->axis = App->renderer3D->IsAxis();
			m->Render();
		}
		++item;
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleGameObjects::RenderChildren(GameObject* parent)
{
	std::vector<GameObject*>::iterator item = parent->children.begin();
	while (item != parent->children.end())
	{
		if (!(*item)->children.empty()) RenderChildren((*item));

		ComponentMesh* m = (*item)->GetComponent<ComponentMesh>();
		if (m != nullptr && m->IsActive())
		{
			(*item) == selectedGameObject ? m->drawBBox = true : m->drawBBox = false;
			m->wireOverride = App->renderer3D->IsWireframe();
			m->axis = App->renderer3D->IsAxis();
			m->Render();
		}
		++item;
	}
}

bool ModuleGameObjects::CleanUp()
{
	selectedGameObject = nullptr;

	LOG("Deleting Game Objects");

	std::vector<GameObject*>::reverse_iterator g = gameObjectList.rbegin();
	while (g != gameObjectList.rend())
	{
		(*g)->CleanUp();
		delete (*g);
		g++;
	}
	gameObjectList.clear();

	return true;
}

void ModuleGameObjects::AddGameobject(GameObject* g)
{
	App->scene->GetSceneRoot()->AddChild(g);
	g->parent = App->scene->GetSceneRoot();

	gameObjectList.push_back(g);
}

void ModuleGameObjects::RemoveGameobject(GameObject* g)
{
	if (App->scene->GetSceneRoot()->RemoveChild(g))
	{
		g->DeleteChildren();
	}
	else if (g->parent->RemoveChild(g))
	{
		g->DeleteChildren();
	}

	for (size_t i = 0; i < gameObjectList.size(); i++)
	{
		if (gameObjectList[i] == g)
		{
			gameObjectList.erase(gameObjectList.begin() + i);
		}
	}

	delete g;
	g = nullptr;
}