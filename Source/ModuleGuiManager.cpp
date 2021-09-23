#include "Globals.h"
#include "Application.h"
#include "ModuleGuiManager.h"

ModuleGuiManager::ModuleGuiManager(Application* app, bool start_enabled): Module(app, start_enabled)
{}

ModuleGuiManager::~ModuleGuiManager()
{}

bool ModuleGuiManager::Init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    return true;
}

update_status ModuleGuiManager::PreUpdate()
{
    return update_status::UPDATE_CONTINUE;
}

update_status ModuleGuiManager::PostUpdate()
{
    return update_status::UPDATE_CONTINUE;
}

bool ModuleGuiManager::CleanUp()
{
    return true;
}

bool ModuleGuiManager::AddWindow(const char* name)
{
    if (ImGui::Begin(name))
    {
        if (name == "Demo")
        {
            ImGui::ShowDemoWindow();
        }
        ImGui::End();
        return true;
    }
    else
    {
        LOG("Error ImGui: Could not open window.");
        return false;
    }
}