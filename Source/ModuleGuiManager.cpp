#include "ModuleGuiManager.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleGuiManager::ModuleGuiManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModuleGuiManager::~ModuleGuiManager()
{}

bool ModuleGuiManager::Start()
{
    LOG("Loading Gui Manager");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& ioHandler = ImGui::GetIO(); (void)ioHandler;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init("#version 460");

    return true;
}

update_status ModuleGuiManager::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(App->window->window);
    ImGui::NewFrame();

    return UPDATE_CONTINUE;
}

update_status ModuleGuiManager::Update()
{
    update_status status;

    status = AddWindow("Demo");
    status = AddWindow("Stop");

    return status;
}

update_status ModuleGuiManager::PostUpdate()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return UPDATE_CONTINUE;
}

bool ModuleGuiManager::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    return true;
}

update_status ModuleGuiManager::AddWindow(const char* name)
{
    if (name == "Demo")
    {
        ImGui::ShowDemoWindow();
    }
    else
    {
        if (ImGui::Begin(name))
        {
            if (name == "Stop")
            {
                if (ImGui::Button("Close App", ImVec2(75.0f, 20.0f)))
                {
                    return UPDATE_STOP;
                }
            }
            ImGui::End();
            return UPDATE_CONTINUE;
        }
        else
        {
            LOG("Error ImGui: Could not open window.");
            return UPDATE_ERROR;
        }
    }
}