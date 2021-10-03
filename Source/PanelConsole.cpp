#include "PanelConsole.h"

#include "Application.h"
#include "ModuleGuiManager.h"

PanelConsole::PanelConsole(Application* app, bool start_enabled) : Panel(app, start_enabled)
{
    name = "console";
}

PanelConsole::~PanelConsole()
{}

update_status PanelConsole::Update()
{
    ImGui::Begin("Console");
    ImGui::TextUnformatted(App->gui->LogConsoleText.begin());
    ImGui::SetScrollHereY(1.0f);
    ImGui::End();
    return UPDATE_CONTINUE;
}