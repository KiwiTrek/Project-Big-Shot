#include "ModuleGuiManager.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleGuiManager::ModuleGuiManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    name = "gui";
    //LogConsoleText.appendf(App->buff->initBuff);
    LogConsoleText.appendf(App->buff->initBuff2);
}

ModuleGuiManager::~ModuleGuiManager()
{
    std::vector<Panel*>::reverse_iterator item = list_panels.rbegin();

    while (item != list_panels.rend())
    {
        delete* item;
        ++item;
    }
}

bool ModuleGuiManager::Start()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& ioHandler = ImGui::GetIO(); (void)ioHandler;
    ioHandler.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ioHandler.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsLight();
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init("#version 460");

    //TODO: Setup Style

    about = new PanelAbout(App);
    console = new PanelConsole(App);
    config = new PanelConfig(App);

    AddPanel(about);
    AddPanel(console);
    AddPanel(config);

    LogConsole(LOG("Loading Gui Manager"));

    demo = false;

    return true;
}

update_status ModuleGuiManager::PreUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(App->window->window);
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    usingKeyboard = io.WantCaptureKeyboard;

    return update_status::UPDATE_CONTINUE;
}

update_status ModuleGuiManager::Update(float dt)
{
    update_status status = update_status::UPDATE_CONTINUE;

    status = MenuBar();

    std::vector<Panel*>::iterator item = list_panels.begin();

    while (item != list_panels.end() && status == update_status::UPDATE_CONTINUE)
    {
        if ((*item)->active == true)
        {
            status = (*item)->Update();
        }
        ++item;
    }

    if (demo) ImGui::ShowDemoWindow();

    return status;
}

update_status ModuleGuiManager::PostUpdate()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return update_status::UPDATE_CONTINUE;
}

bool ModuleGuiManager::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    return true;
}

void ModuleGuiManager::AddPanel(Panel* panel)
{
    list_panels.push_back(panel);
}

update_status ModuleGuiManager::MenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Quit","ESC"))
            {
                return update_status::UPDATE_STOP;
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Configuration", "F1", &config->active);
            ImGui::MenuItem("Console", "F12", &console->active);

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

            ImGui::MenuItem("About", nullptr, &about->active);

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

    return update_status::UPDATE_CONTINUE;
}

void ModuleGuiManager::LogConsole(const char* buff)
{
    LogConsoleText.appendf(buff);
}