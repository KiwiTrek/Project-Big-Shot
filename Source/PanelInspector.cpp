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
	GameObject* g = App->gameObjects->selectedGameObject;
	if (g != nullptr)
	{
		std::string name = g->name;
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
		flags &= ~ImGuiInputTextFlags_ReadOnly;
		ImGui::InputText("Name",&name,flags);
		std::vector<Component*>::iterator item = g->components.begin();
		while (item != g->components.end())
		{
			(*item)->DrawInspector();
			item++;
		}
	}
	else
	{
		ImGui::TextWrapped("Select a game object to see its components.");
	}
	ImGui::End();

	return update_status::UPDATE_CONTINUE;
}