#include "PanelConsole.h"
#include "Application.h"
#include "ModuleGuiManager.h"

PanelConsole::PanelConsole(Application* app, bool startEnabled) : Panel(app, startEnabled)
{
	name = "Console";
}

PanelConsole::~PanelConsole()
{}

UpdateStatus PanelConsole::Update()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	ImGui::TextUnformatted(App->gui->LogConsoleText.begin());
	if (update)
	{
		update = false;
		ImGui::SetScrollHereY(1.0f);
	}

	ImGui::End();
	return UpdateStatus::UPDATE_CONTINUE;
}