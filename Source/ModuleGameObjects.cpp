#include "ModuleGameObjects.h"
#include "Application.h"

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

UpdateStatus ModuleGameObjects::PostUpdate()
{
	Mesh* gridMesh = nullptr;
	std::vector<Component*>::iterator gridComponent = App->scene->grid->components.begin();
	while (gridComponent != App->scene->grid->components.end())
	{
		if ((*gridComponent)->type == ComponentTypes::MESH)
		{
			gridMesh = (Mesh*)(*gridComponent);
			break;
		}
		gridComponent++;
	}

	if (gridMesh != nullptr && gridMesh->IsActive())
	{
		gridMesh->wire = App->renderer3D->IsWireframe();
		gridMesh->Render();
	}

	std::vector<GameObject*>::iterator item = gameObjectList.begin();
	while (item != gameObjectList.end())
	{
		Mesh* m = nullptr;

		if (!(*item)->children.empty()) RenderChildren((*item));

		std::vector<Component*>::iterator c = (*item)->components.begin();
		while (c != (*item)->components.end())
		{
			if ((*c)->type == ComponentTypes::MESH)
			{
				m = (Mesh*)(*c);
				break;
			}
			c++;
		}

		if (m != nullptr && m->IsActive())
		{
			m->wire = App->renderer3D->IsWireframe();
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
		Mesh* m = nullptr;

		if (!(*item)->children.empty()) RenderChildren((*item));

		std::vector<Component*>::iterator c = (*item)->components.begin();
		while (c != (*item)->components.end())
		{
			if ((*c)->type == ComponentTypes::MESH) m = (Mesh*)(*c);
			c++;
		}

		if (m != nullptr && m->IsActive())
		{
			if (m->GetType() != MeshTypes::Primitive_Grid) m->axis = App->renderer3D->IsAxis();

			m->wire = App->renderer3D->IsWireframe();
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