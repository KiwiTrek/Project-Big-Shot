#include "PanelInspector.h"
#include "ModuleGuiManager.h"

#include "Application.h"
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
		if (ImGui::InputText("Name", &name, flags)) g->name = name;
		IMGUI_PRINT(IMGUI_YELLOW, "UID: ", "%d", g->uid);
		ImGui::Checkbox("active: ",&g->active);
		std::vector<Component*>::iterator item = g->components.begin();
		while (item != g->components.end())
		{
			(*item)->DrawInspector(App);
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