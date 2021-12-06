#include "ModuleGameObjects.h"
#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
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
	LOG_CONSOLE("Initializing Gizmo Handler & GameObjects");
	ImGuizmo::Enable(true);

	currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	currentGizmoMode = ImGuizmo::MODE::WORLD;
	return true;
}

bool ModuleGameObjects::Start()
{
	return true;
}

UpdateStatus ModuleGameObjects::PreUpdate()
{
	if ((App->input->GetKey(SDL_SCANCODE_T) == KeyState::KEY_DOWN))
	{
		currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
		LOG_CONSOLE("Set Guizmo to Translate");
	}
	else if ((App->input->GetKey(SDL_SCANCODE_Y) == KeyState::KEY_DOWN))
	{
		currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
		LOG_CONSOLE("Set Guizmo to Rotate");
	}
	else if ((App->input->GetKey(SDL_SCANCODE_U) == KeyState::KEY_DOWN))
	{
		currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
		LOG_CONSOLE("Set Guizmo to Scale");
	}

	return UpdateStatus::UPDATE_CONTINUE;
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
			if ((*item) == selectedGameObject && (*c)->type == ComponentTypes::MESH)
			{
				ComponentMesh* cm = (ComponentMesh*)(*c);
				cm->UpdateBBox();
			}
			c++;
		}
		item++;
	}

	GuizmoTransformation();

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
			if ((*item) == selectedGameObject && (*c)->type == ComponentTypes::MESH)
			{
				ComponentMesh* cm = (ComponentMesh*)(*c);
				if (cm->mesh != nullptr) cm->UpdateBBox();
			}
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

		ComponentMesh* m = (*item)->GetComponent<Mesh>();
		if (m != nullptr && m->IsActive())
		{
			(*item) == selectedGameObject ? m->drawBBox = true : m->drawBBox = false;
			m->wireOverride = App->renderer3D->IsWireframe();

			if (mainCamera != nullptr)
			{
				if (mainCamera->GetComponent<Camera>()->culling == true)
				{
					mainCamera->GetComponent<Camera>()->ContainsBBox(m->bbox) ? m->render = true : m->render = false;
				}
				else
				{
					m->render = true;
				}
			}
			else
			{
				m->render = true;
			}

			m->Render();
			if (m->drawBBox) m->DrawBBox();
		}

		ComponentCamera* c = (*item)->GetComponent<Camera>();
		if (c != nullptr && c->IsActive())
		{
			c->Render();
			if ((*item) == selectedGameObject)
			{
				c->drawFrustum = true;
				c->DrawBBox();
			}
			else
			{
				c->drawFrustum = false;
			}
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

		ComponentMesh* m = (*item)->GetComponent<Mesh>();
		if (m != nullptr && m->IsActive())
		{
			(*item) == selectedGameObject ? m->drawBBox = true : m->drawBBox = false;
			m->wireOverride = App->renderer3D->IsWireframe();

			if (mainCamera != nullptr)
			{
				if (mainCamera->GetComponent<Camera>()->culling == true)
				{
					mainCamera->GetComponent<Camera>()->ContainsBBox(m->bbox) ? m->render = true : m->render = false;
				}
				else
				{
					m->render = true;
				}
			}
			else
			{
				m->render = true;
			}
			m->Render();
			if (m->drawBBox) m->DrawBBox();

			ComponentCamera* c = (*item)->GetComponent<Camera>();
			if (c != nullptr && c->IsActive())
			{
				(*item) == selectedGameObject ? c->drawFrustum = true : c->drawFrustum = false;
			}
		}
		++item;
	}
}

void ModuleGameObjects::GuizmoTransformation()
{
	if (selectedGameObject == nullptr)
		return;

	float4x4 viewMatrix = App->camera->viewMatrix.Transposed();
	float4x4 projectionMatrix = App->camera->cameraFrustum.ProjectionMatrix().Transposed();
	float4x4 objectTransform = selectedGameObject->GetComponent<Transform>()->GetGlobalTransform().Transposed();

	float tempTransform[16];
	memcpy(tempTransform, objectTransform.ptr(), 16 * sizeof(float));

	int winX, winY;
	App->window->GetPosition(winX, winY);
	ImGuizmo::SetRect(App->gui->scenePanelOrigin.x + winX, App->gui->scenePanelOrigin.y + winY, App->gui->viewportSize.x, App->gui->viewportSize.y);
	ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), currentGizmoOperation, currentGizmoOperation != ImGuizmo::OPERATION::SCALE ? currentGizmoMode : ImGuizmo::MODE::LOCAL, tempTransform);

	if (ImGuizmo::IsUsing())
	{
		float4x4 newTransform;
		newTransform.Set(tempTransform);
		objectTransform = newTransform.Transposed();
		selectedGameObject->GetComponent<Transform>()->SetGlobalTransform(objectTransform);
	}
}

bool ModuleGameObjects::CleanUp()
{
	selectedGameObject = nullptr;

	LOG("Deleting Game Objects");
	ImGuizmo::Enable(false);

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

std::vector<GameObject*> ModuleGameObjects::GetAllGameObjects()
{
	std::vector<GameObject*> gameObjects;

	PreorderGameObjects(App->scene->GetSceneRoot(), gameObjects);

	return gameObjects;
}

void ModuleGameObjects::PreorderGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects)
{
	gameObjects.push_back(gameObject);

	for (size_t i = 0; i < gameObject->children.size(); i++)
	{
		PreorderGameObjects(gameObject->GetChildAt(i), gameObjects);
	}
}