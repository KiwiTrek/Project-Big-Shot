#include "ModuleGuiManager.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleGuiManager::ModuleGuiManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    name = "gui";
    //LogConsoleText.appendf(App->buff->initBuff);
    LogConsoleText.appendf(App->buff->initBuff2);

    about = new PanelAbout(App);
    console = new PanelConsole(App);
    config = new PanelConfig(App);

    AddPanel(about);
    AddPanel(console);
    AddPanel(config);
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
    ioHandler.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init("#version 460");

    SetupStyle();

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

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    windowFlags |= ImGuiWindowFlags_NoBackground;
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    if (ImGui::Begin("Dockspace", 0, windowFlags))
    {
        // DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceId = ImGui::GetID("DefaultDockspace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        }
    }
    ImGui::End();
    ImGui::PopStyleVar(3);

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
    SDL_GL_MakeCurrent(App->window->window, App->renderer3D->context);
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

void ModuleGuiManager::SetupStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 10);
    style->WindowRounding = 2.5f;
    style->ChildBorderSize = 1.0f;
    style->ChildRounding = 2.5f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 2.0f;
    style->ItemSpacing = ImVec2(10, 5);
    style->ItemInnerSpacing = ImVec2(8, 5);
    style->IndentSpacing = 20.0f;
    style->ScrollbarSize = 18.0f;
    style->ScrollbarRounding = 1.0f;
    style->DisplaySafeAreaPadding = ImVec2(0, 0);
    style->DisplayWindowPadding = ImVec2(0, 0);

    ImGui::StyleColorsDark();
    ImVec4* colors = style->Colors;
    colors[ImGuiCol_Text] = IMGUI_WHITE;
    colors[ImGuiCol_TextDisabled] = IMGUI_GREY;
    colors[ImGuiCol_TextSelectedBg] = IMGUI_LIGHT_PINK;
    colors[ImGuiCol_WindowBg] = IMGUI_DARK_PINK;
    colors[ImGuiCol_ChildBg] = IMGUI_DARK_PINK;
    colors[ImGuiCol_PopupBg] = IMGUI_DARK_PINK;
    colors[ImGuiCol_Border] = IMGUI_LIGHT_GREY;
    colors[ImGuiCol_BorderShadow] = IMGUI_BLACK;
    colors[ImGuiCol_FrameBg] = IMGUI_LIGHT_PINK;
    colors[ImGuiCol_FrameBgHovered] = IMGUI_PINK;
    colors[ImGuiCol_FrameBgActive] = IMGUI_DARK_YELLOW;
    colors[ImGuiCol_TitleBg] = IMGUI_BLACK;
    colors[ImGuiCol_TitleBgActive] = IMGUI_DARK_PURPLE;
    colors[ImGuiCol_TitleBgCollapsed] = IMGUI_GREY;
    colors[ImGuiCol_MenuBarBg] = IMGUI_BLACK;
    colors[ImGuiCol_ScrollbarBg] = IMGUI_LIGHT_PURPLE;
    colors[ImGuiCol_ScrollbarGrab] = IMGUI_DARK_PURPLE;
    colors[ImGuiCol_ScrollbarGrabHovered] = IMGUI_PURPLE;
    colors[ImGuiCol_ScrollbarGrabActive] = IMGUI_YELLOW;
    colors[ImGuiCol_CheckMark] = IMGUI_DARK_PINK;
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_Button] = IMGUI_PINK;
    colors[ImGuiCol_ButtonHovered] = IMGUI_LIGHT_PINK;
    colors[ImGuiCol_ButtonActive] = IMGUI_DARK_YELLOW;
    colors[ImGuiCol_Header] = IMGUI_PINK;
    colors[ImGuiCol_HeaderHovered] = IMGUI_LIGHT_PINK;
    colors[ImGuiCol_HeaderActive] = IMGUI_DARK_YELLOW;
    colors[ImGuiCol_Separator] = IMGUI_LIGHT_GREEN;
    colors[ImGuiCol_SeparatorHovered] = IMGUI_YELLOW;
    colors[ImGuiCol_SeparatorActive] = IMGUI_DARK_YELLOW;
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
    colors[ImGuiCol_ResizeGripActive] = IMGUI_YELLOW;
    colors[ImGuiCol_Tab] = IMGUI_PURPLE;
    colors[ImGuiCol_TabHovered] = IMGUI_LIGHT_PURPLE;
    colors[ImGuiCol_TabActive] = IMGUI_LIGHT_PURPLE;
    colors[ImGuiCol_TabUnfocused] = IMGUI_WHITE;
    colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
    colors[ImGuiCol_DockingPreview] = IMGUI_LIGHT_GREEN;
    colors[ImGuiCol_DockingEmptyBg] = IMGUI_WHITE;
    colors[ImGuiCol_PlotLines] = IMGUI_PINK;
    colors[ImGuiCol_PlotLinesHovered] = IMGUI_YELLOW;
    colors[ImGuiCol_PlotHistogram] = IMGUI_PINK;
    colors[ImGuiCol_PlotHistogramHovered] = IMGUI_YELLOW;
    colors[ImGuiCol_TableHeaderBg] = IMGUI_PINK;
    colors[ImGuiCol_TableBorderStrong] = IMGUI_PINK;   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableBorderLight] = IMGUI_LIGHT_PINK;   // Prefer using Alpha=1.0 here
    colors[ImGuiCol_TableRowBg] = IMGUI_BLACK;
    colors[ImGuiCol_TableRowBgAlt] = IMGUI_DARK_PURPLE;
    colors[ImGuiCol_DragDropTarget] = IMGUI_LIGHT_GREEN;
    colors[ImGuiCol_NavHighlight] = IMGUI_YELLOW;
    colors[ImGuiCol_NavWindowingHighlight] = IMGUI_GREY;
    colors[ImGuiCol_NavWindowingDimBg] = IMGUI_LIGHT_GREY;
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}