#include "PanelConsole.h"
#include "Application.h"
#include "ModuleGuiManager.h"

PanelConsole::PanelConsole(Application* app, bool start_enabled) : Panel(app, start_enabled)
{
    name = "Console";
}

PanelConsole::~PanelConsole()
{}

update_status PanelConsole::Update()
{
    ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    ImGui::TextUnformatted(App->gui->LogConsoleText.begin());
    if (update)
    {
        update = false;
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::End();
    return update_status::UPDATE_CONTINUE;
}