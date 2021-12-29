#include "PanelInspector.h"
#include "ModuleGuiManager.h"

#include "Application.h"
#include "ModuleGameObjects.h"
#include "ModuleScene.h"

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
		if (g == App->scene->GetSceneRoot()) flags |= ImGuiInputTextFlags_ReadOnly;
		if (ImGui::InputText("Name", &name, flags)) g->name = name;
		IMGUI_PRINT(IMGUI_YELLOW, "UID: ", "%d", g->uid);
		for (std::vector<Component*>::iterator item = g->components.begin(); item != g->components.end(); item++)
		{
			(*item)->DrawInspector(App);
		}
	}
	else
	{
		ImGui::TextWrapped("Select a game object to see its components.");
	}

	ImGui::End();
	return UpdateStatus::UPDATE_CONTINUE;
}