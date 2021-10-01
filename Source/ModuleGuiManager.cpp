#include "ModuleGuiManager.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleGuiManager::ModuleGuiManager(Application* app, bool start_enabled) : Module(app, start_enabled), fpsHist(100), msHist(100)
{}

ModuleGuiManager::~ModuleGuiManager()
{}

bool ModuleGuiManager::Start()
{
    LOG("Loading Gui Manager");
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& ioHandler = ImGui::GetIO(); (void)ioHandler;
    ioHandler.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsLight();
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init("#version 460");

    config = false;
    console = false;
    about = false;
    demo = false;

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
    update_status status = UPDATE_CONTINUE;

    status = MenuBar();
    if (demo) ImGui::ShowDemoWindow();
    if (config) Config();

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

update_status ModuleGuiManager::MenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit","ESC"))
            {
                return UPDATE_STOP;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Configuration", "F1", &config);
            ImGui::MenuItem("Console", "F12", &console);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("Gui Demo", nullptr, &demo);

            if (ImGui::MenuItem("Source"))
            {
                App->RequestBrowser("https://github.com/WillyTrek19/Project-Big-Shot");
            }

            if (ImGui::MenuItem("Download Latest Release"))
            {
                App->RequestBrowser("https://github.com/WillyTrek19/Project-Big-Shot/releases");
            }

            if (ImGui::MenuItem("Report a bug"))
            {
                App->RequestBrowser("https://github.com/WillyTrek19/Project-Big-Shot/issues");
            }

            ImGui::MenuItem("About", nullptr, &about);

            ImGui::EndMenu();
        }

    }
    ImGui::EndMainMenuBar();

    return UPDATE_CONTINUE;
}

void ModuleGuiManager::Config()
{
    if (ImGui::Begin("Configuration"))
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("Set Defaults"))
            {

            }
            if (ImGui::MenuItem("Save"))
            {

            }
            if (ImGui::MenuItem("Load"))
            {

            }

            ImGui::EndMenu();
        }

        if (ImGui::CollapsingHeader("Application"))
        {
            static char appName[120];
            strcpy_s(appName, 120, App->GetAppName());
            if (ImGui::InputText("App Name", appName, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
            {
                App->SetAppName(appName);
            }

            static char orgName[120];
            strcpy_s(orgName, 120, App->GetOrgName());
            if (ImGui::InputText("Organization", orgName, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
            {
                App->SetOrgName(orgName);
            }

            int maxFPS = App->GetFpsLimit();
            if (ImGui::SliderInt("Max FPS", &maxFPS, 1, 120))
            {
                App->SetFpsLimit(maxFPS);
            }

            //TODO: should only get the last frame framerate
            std::string Title = "Framerate: " + std::to_string(App->GetFps());
            ImGui::PlotHistogram("##framerate", &fpsHist[0], fpsHist.size(), 0, Title.c_str(), 0.0f, 120.0f, ImVec2(ImGui::CalcItemWidth(), 100.0f));
            Title = "Milliseconds: " + std::to_string(App->GetMs());
            ImGui::PlotHistogram("##milliseconds", &msHist[0], msHist.size(), 0, Title.c_str(), 0.0f, 50.0f, ImVec2(ImGui::CalcItemWidth(), 100.0f));
        }

        if (ImGui::CollapsingHeader("Window"))
        {
            float b = App->window->GetBrightness();
            if (ImGui::SliderFloat("Brightness", &b, 0.05f, 1.0f))
            {
                App->window->SetBrightness(b);
            }

            int w = App->window->GetWidth();
            int h = App->window->GetHeight();
            int maxW, maxH;
            App->window->GetMaxWindow(maxW, maxH);
            if (ImGui::SliderInt("Width", &w, 640, maxW))
            {
                App->window->SetWidth(w);
            }

            if (ImGui::SliderInt("Height", &h, 480, maxH))
            {
                App->window->SetHeight(h);
            }

            ImGui::Text("Refresh rate:");
            ImGui::SameLine();
            ImGui::TextColored(IMGUI_BLUE, "%u", App->window->GetRefreshRate());

            bool fullscreen = App->window->IsFullscreen();
            if (ImGui::Checkbox("Fullscreen", &fullscreen))
            {
                App->window->SetFullscreen(fullscreen);
            }
            bool resizable = App->window->IsResizable();
            if(ImGui::Checkbox("Resizable", &resizable))
            {
                App->window->SetResizable(resizable);
            }
            bool borderless = App->window->IsBorderless();
            if (ImGui::Checkbox("Borderless", &borderless))
            {
                App->window->SetBorderless(borderless);
            }
            bool fullscreenDesktop = App->window->IsFullscreenDesktop();
            if (ImGui::Checkbox("Fullscreen Desktop", &fullscreenDesktop))
            {
                App->window->SetFullscreenDesktop(fullscreenDesktop);
            }
        }

        ImGui::End();
    }
}

void ModuleGuiManager::UpdateHistogram()
{
    static uint count = 0;

    if (count == 100)
    {
        for (uint i = 0; i < 100 - 1; ++i)
        {
            fpsHist[i] = fpsHist[i + 1];
            msHist[i] = msHist[i + 1];
        }
    }
    else
        ++count;

    fpsHist[count - 1] = App->GetFps();
    msHist[count - 1] = App->GetMs();
}