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
	if (selectedGameObject != nullptr && selectedGameObject->GetComponent<Camera>() != nullptr && currentGizmoOperation == ImGuizmo::OPERATION::SCALE)
		currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;

	if (App->gui->MouseOnScene() && (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::KEY_REPEAT))
	{
		if ((App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_DOWN))
		{
			currentGizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
			LOG_CONSOLE("Set Guizmo to Translate");
		}
		if ((App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_DOWN))
		{
			currentGizmoOperation = ImGuizmo::OPERATION::ROTATE;
			LOG_CONSOLE("Set Guizmo to Rotate");
		}
		if (selectedGameObject != nullptr && selectedGameObject->GetComponent<Camera>() == nullptr)
		{
			if ((App->input->GetKey(SDL_SCANCODE_R) == KeyState::KEY_DOWN))
			{
				currentGizmoOperation = ImGuizmo::OPERATION::SCALE;
				LOG_CONSOLE("Set Guizmo to Scale");
			}
		}
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleGameObjects::Update(float dt)
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	for(std::vector<GameObject*>::iterator item = gameObjectList.begin(); item != gameObjectList.end(); item++)
	{
		if (!(*item)->children.empty()) ret = UpdateChildren(dt, (*item));
		std::vector<Component*>::iterator c = (*item)->components.begin();
		while (c != (*item)->components.end())
		{
			if((*c)->IsActive()) (*c)->Update(dt, App);
			if ((*item) == selectedGameObject && (*c)->type == ComponentTypes::MESH)
			{
				ComponentMesh* cm = (ComponentMesh*)(*c);
				cm->UpdateBBox();
			}
			c++;
		}
	}

	GuizmoTransformation();

	return ret;
}

UpdateStatus ModuleGameObjects::UpdateChildren(float dt, GameObject* parent)
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	for (std::vector<GameObject*>::iterator item = parent->children.begin(); item != parent->children.end(); ++item)
	{
		if (!(*item)->children.empty()) ret = UpdateChildren(dt, (*item));
		std::vector<Component*>::iterator c = (*item)->components.begin();
		while (c != (*item)->components.end())
		{
			if((*c)->IsActive()) (*c)->Update(dt, App);
			if ((*item) == selectedGameObject && (*c)->type == ComponentTypes::MESH)
			{
				ComponentMesh* cm = (ComponentMesh*)(*c);
				if (cm->mesh != nullptr) cm->UpdateBBox();
			}
			c++;
		}
	}
	return ret;
}

UpdateStatus ModuleGameObjects::PostUpdate()
{
	for(std::vector<GameObject*>::iterator item = gameObjectList.begin(); item != gameObjectList.end(); ++item)
	{
		if (!(*item)->children.empty()) RenderChildren((*item));

		//Mesh
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

			if (m->render) m->Render();
			if (m->drawBBox) m->DrawBBox();
		}

		//Camera
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
	}

	return UpdateStatus::UPDATE_CONTINUE;
}


void ModuleGameObjects::RenderChildren(GameObject* parent)
{
	for(std::vector<GameObject*>::iterator item = parent->children.begin(); item != parent->children.end(); ++item)
	{
		if (!(*item)->children.empty()) RenderChildren((*item));

		//Mesh
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
			if (m->render) m->Render();
			if (m->drawBBox) m->DrawBBox();
		}

		//Camera
		ComponentCamera* c = (*item)->GetComponent<Camera>();
		if (c != nullptr && c->IsActive())
		{
			(*item) == selectedGameObject ? c->drawFrustum = true : c->drawFrustum = false;
		}
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
	LOG("Deleting Game Objects");
	ImGuizmo::Enable(false);

	for(std::vector<GameObject*>::reverse_iterator g = gameObjectList.rbegin(); g != gameObjectList.rend(); ++g)
	{
		(*g)->CleanUp();
		delete (*g);
	}
	gameObjectList.clear();

	selectedGameObject = nullptr;
	mainCamera = nullptr;

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
		if (gameObjectList[i] == g) gameObjectList.erase(gameObjectList.begin() + i);
	}

	delete g;
	g = nullptr;
}

std::vector<GameObject*> ModuleGameObjects::GetAllGameObjects()
{
	std::vector<GameObject*> gameObjects;

	OrderGameObjects(App->scene->GetSceneRoot(), gameObjects);

	return gameObjects;
}

void ModuleGameObjects::OrderGameObjects(GameObject* gameObject, std::vector<GameObject*>& gameObjects)
{
	gameObjects.push_back(gameObject);

	for (size_t i = 0; i < gameObject->children.size(); i++)
	{
		OrderGameObjects(gameObject->GetChildAt(i), gameObjects);
	}
}