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
	std::vector<GameObject*>::iterator item = gameobjectList.begin();

	while (item != gameobjectList.end())
	{
		Mesh* m = nullptr;

		if ((*item)->children.empty())
		{
			std::vector<Component*>::iterator c = (*item)->components.begin();
			while (c != (*item)->components.end())
			{
				if ((*c)->type == ComponentTypes::MESH)
				{
					m = (Mesh*)(*c);
				}
				c++;
			}
		}
		else
		{
			RenderChildren((*item));
		}

		if (m != nullptr)
		{
			m->wire = App->renderer3D->IsWireframe();
			m->drawFaceNormals = App->renderer3D->IsFaceNormals();
			m->drawVertexNormals = App->renderer3D->IsVertexNormals();
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

		if ((*item)->children.empty())
		{
			std::vector<Component*>::iterator c = (*item)->components.begin();
			while (c != (*item)->components.end())
			{
				if ((*c)->type == ComponentTypes::MESH)
				{
					m = (Mesh*)(*c);
				}
				c++;
			}
		}
		else
		{
			RenderChildren((*item));
		}

		if (m != nullptr)
		{
			if (m->GetType() != MeshTypes::Primitive_Grid)
			{
				m->axis = App->renderer3D->IsAxis();
			}
			m->wire = App->renderer3D->IsWireframe();
			m->drawFaceNormals = App->renderer3D->IsFaceNormals();
			m->drawVertexNormals = App->renderer3D->IsVertexNormals();
			m->Render();
		}

		++item;
	}
}

bool ModuleGameObjects::CleanUp()
{
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

	delete g;
	g = nullptr;
}