#include "PanelInspector.h"

#include "Application.h"
#include "ModuleGuiManager.h"
#include "ModuleGameObjects.h"

#include "Gameobject.h"
#include "Component.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

PanelInspector::PanelInspector(Application* app, bool start_enabled) : Panel(app, start_enabled)
{
	name = "Inspector";
}

PanelInspector::~PanelInspector()
{}

update_status PanelInspector::Update()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	if (App->gameObjects->selectedGameObject != nullptr)
	{
		std::vector<Component*>::iterator item = App->gameObjects->selectedGameObject->components.begin();
		while (item != App->gameObjects->selectedGameObject->components.end())
		{
			(*item)->DrawInspector();
			item++;
		}
	}
	else
	{
		ImGui::Text("Select a game object to see its components.");
	}
	ImGui::End();

	return update_status::UPDATE_CONTINUE;
}