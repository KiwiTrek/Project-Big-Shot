#include "PanelInspector.h"

#include "Application.h"
#include "ModuleGuiManager.h"
#include "ModuleGameObjects.h"

PanelInspector::PanelInspector(Application* app, bool startEnabled) : Panel(app, startEnabled)
{
	name = "Inspector";
}

PanelInspector::~PanelInspector()
{}

UpdateStatus PanelInspector::Update()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	GameObject* g = App->gameObjects->selectedGameObject;
	if (g != nullptr)
	{
		std::string name = g->name;
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
		flags &= ~ImGuiInputTextFlags_ReadOnly;
		ImGui::InputText("Name", &name, flags);
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
	return UpdateStatus::UPDATE_CONTINUE;
}