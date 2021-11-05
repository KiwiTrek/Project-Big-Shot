#include "ModuleGameObjects.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Gameobject.h"

ModuleGameObjects::ModuleGameObjects(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    name = "game_objects";
}

ModuleGameObjects::~ModuleGameObjects()
{

}

bool ModuleGameObjects::Init()
{
    return true;
}

bool ModuleGameObjects::Start()
{
    return true;
}

update_status ModuleGameObjects::PostUpdate()
{

	std::vector<Component*>::iterator gridComponent = App->scene->grid->components.begin();
	Mesh* gridMesh = nullptr;
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

	std::vector<GameObject*>::iterator item = gameobjectList.begin();

	while (item != gameobjectList.end())
	{
		Mesh* m = nullptr;

		if (!(*item)->children.empty())
		{
			RenderChildren((*item));
		}

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
			m->Render();
		}

		++item;
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleGameObjects::RenderChildren(GameObject* parent)
{
	std::vector<GameObject*>::iterator item = parent->children.begin();
	while (item != parent->children.end())
	{
		Mesh* m = nullptr;

		if (!(*item)->children.empty())
		{
			RenderChildren((*item));
		}

		std::vector<Component*>::iterator c = (*item)->components.begin();
		while (c != (*item)->components.end())
		{
			if ((*c)->type == ComponentTypes::MESH)
			{
				m = (Mesh*)(*c);
			}
			c++;
		}

		if (m != nullptr && m->IsActive())
		{
			if (m->GetType() != MeshTypes::Primitive_Grid)
			{
				m->axis = App->renderer3D->IsAxis();
			}
			m->wire = App->renderer3D->IsWireframe();
			m->Render();
		}

		++item;
	}
}

bool ModuleGameObjects::CleanUp()
{
	selectedGameObject = nullptr;

	std::vector<GameObject*>::reverse_iterator g = gameobjectList.rbegin();
	while (g != gameobjectList.rend())
	{
		(*g)->CleanUp();
		delete (*g);
		g++;
	}
	gameobjectList.clear();

    return true;
}

void ModuleGameObjects::AddGameobject(GameObject* g)
{
	App->scene->GetSceneRoot()->AddChild(g);
	g->SetParent(App->scene->GetSceneRoot());

	gameobjectList.push_back(g);
}

void ModuleGameObjects::RemoveGameobject(GameObject* g)
{
	if (App->scene->GetSceneRoot()->RemoveChild(g))
	{
		g->DeleteChildren();
	}
	else if (g->GetParent()->RemoveChild(g))
	{
		g->DeleteChildren();
	}

	for (size_t i = 0; i < gameobjectList.size(); i++)
	{
		if (gameobjectList[i] == g)
		{
			gameobjectList.erase(gameobjectList.begin() + i);
		}
	}

	delete g;
	g = nullptr;
}