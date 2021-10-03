#include "ModuleGuiManager.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleGuiManager::ModuleGuiManager(Application* app, bool start_enabled) : Module(app, start_enabled), fpsHist(100), msHist(100)
{
    name = "gui";
}

ModuleGuiManager::~ModuleGuiManager()
{}

bool ModuleGuiManager::Start()
{
    if (App->gui != nullptr) App->gui->LogConsole(LOG("Loading Gui Manager"));
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& ioHandler = ImGui::GetIO(); (void)ioHandler;
    ioHandler.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsLight();
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init("#version 460");

    LogConsoleText.appendf(App->buff->initBuff1);
    LogConsoleText.appendf(App->buff->initBuff2);

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
    if (console) Console();
    if (about) About();

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
    ImGui::Begin("Configuration");
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
        //TODO: ImGui::InputText does not work
        static char appName[120];
        strcpy_s(appName, 120, App->GetAppName());
        if (ImGui::InputText("App Name", appName, IM_ARRAYSIZE(appName), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
        {
            App->SetAppName(appName);
        }

        static char orgName[120];
        strcpy_s(orgName, 120, App->GetOrgName());
        if (ImGui::InputText("Organization", orgName, IM_ARRAYSIZE(orgName), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
        {
            App->SetOrgName(orgName);
        }
        ImGui::Separator();

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
        ImGui::Separator();
        bool fullscreen = App->window->IsFullscreen();
        if (ImGui::Checkbox("Fullscreen", &fullscreen))
        {
            App->window->SetFullscreen(fullscreen);
        }
        bool resizable = App->window->IsResizable();
        if (ImGui::Checkbox("Resizable", &resizable))
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

    if (ImGui::CollapsingHeader("Hardware"))
    {
        int major, minor, patch;
        App->GetSDLVersion(major, minor, patch);
        IMGUI_PRINT(IMGUI_BLUE, "SDL Version:", "%d.%d.%d", major, minor, patch);
        ImGui::Separator();
        int count, size;
        App->GetCPU(count, size);
        IMGUI_PRINT(IMGUI_BLUE, "CPUs:", "%d (%dKb)", count, size);
        float ram = App->GetRAM();
        IMGUI_PRINT(IMGUI_BLUE, "RAM:", "%.2fGb", ram);
        ImGui::Separator();
        bool threeD, altiVec, avx, avx2, mmx, rdtsc, sse, sse2, sse3, sse41, sse42;
        App->GetCaps(threeD, altiVec, avx, avx2, mmx, rdtsc, sse, sse2, sse3, sse41, sse42);
        IMGUI_PRINT(IMGUI_BLUE, "Caps:", "%s%s%s%s%s%s", threeD ? "3DNow, " : "", altiVec ? "AltiVec, " : "", avx ? "AVX, " : "", avx2 ? "AVX2, " : "", mmx ? "MMX, " : "", rdtsc ? "RDTSC, " : "");
        IMGUI_PRINT(IMGUI_BLUE, "", "%s%s%s%s%s", sse ? "SSE, " : "", sse2 ? "SSE2, " : "", sse3 ? "SSE3, " : "", sse41 ? "SSE41, " : "", sse42 ? "SSE42" : "");

        ImGui::Separator();
        uint vendorId, deviceId;
        char brand[250];
        float videoMemBudget, videoMemCurrent, videoMemAvailable, videoMemReserved;
        App->GetGPU(vendorId, deviceId, brand, videoMemBudget, videoMemCurrent, videoMemAvailable, videoMemReserved);
        IMGUI_PRINT(IMGUI_BLUE, "GPU: ", "VendorId: %d - DeviceId: %d");
        IMGUI_PRINT(IMGUI_BLUE, "Brand: ", brand);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Budget: ", "%.1f Mb", videoMemBudget);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Usage: ", "%.1f Mb", videoMemCurrent);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Available: ", "%.1f Mb", videoMemAvailable);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Reserved: ", "%.1f Mb", videoMemReserved);

    }

    if (ImGui::CollapsingHeader("Render"))
    {
        bool vSync = App->renderer3D->GetVSync();
        if (ImGui::Checkbox("VSync", &vSync))
        {
            App->renderer3D->SetVSync(vSync);
        }
        IMGUI_PRINT(IMGUI_BLUE, "Video Driver:", "%s", App->renderer3D->GetVideoDriver());
    }

    if (ImGui::CollapsingHeader("Input"))
    {
        ImVec2 mousePos;
        mousePos.x = App->input->GetMouseX();
        mousePos.y = App->input->GetMouseY();
        IMGUI_PRINT(IMGUI_BLUE, "Mouse Position: ", "%d,%d", (int)mousePos.x, (int)mousePos.y);
        int mousewheel = App->input->GetMouseZ();
        IMGUI_PRINT(IMGUI_BLUE, "Mousewheel: ", "%d", mousewheel);
        ImVec2 mouseMotion;
        mouseMotion.x = App->input->GetMouseXMotion();
        mouseMotion.y = App->input->GetMouseYMotion();
        IMGUI_PRINT(IMGUI_BLUE, "Mouse Motion: ", "%.2f,%.2f", mouseMotion.x, mouseMotion.y);
        ImGui::Separator();
        if (ImGui::Button("Clear", ImVec2(ImGui::CalcItemWidth(), 20)))
        {
            LogInputText.clear();
        }
        ImGui::BeginChild("Input Log");
        ImGui::TextUnformatted(LogInputText.begin());
        ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
    ImGui::End();
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

void ModuleGuiManager::Console()
{
    ImGui::Begin("Console");
    ImGui::TextUnformatted(LogConsoleText.begin());
    ImGui::SetScrollHereY(1.0f);
    ImGui::End();
}

void ModuleGuiManager::LogConsole(const char* buff)
{
    LogConsoleText.appendf(buff);
}

void ModuleGuiManager::About()
{
    ImGui::Begin("About");
    ImGui::SetWindowFontScale(1.2f);
    ImGui::TextColored(IMGUI_BLUE, "Project Big Shot");
    ImGui::SetWindowFontScale(1);
    ImGui::TextWrapped("Engine created for the 'Game Engines' subject in the Bachelor's degree in Video Game Design and Development at the CITM-UPC center, Barcelona.");
    ImGui::Text("By:");
    ImGui::BulletText("Guillem Alava (https://github.com/WillyTrek19)");
    ImGui::SameLine();
    if (ImGui::Button("Go to link",ImVec2(100,20)))
    {
        App->RequestBrowser("https://github.com/WillyTrek19");
    }
    ImGui::BulletText("Sergi Colomer (https://github.com/Lladruc37)");
    ImGui::SameLine();
    if (ImGui::Button("Go to link", ImVec2(100, 20)))
    {
        App->RequestBrowser("https://github.com/Lladruc37");
    }

    ImGui::Separator();
    ImGui::TextColored(IMGUI_BLUE, "3rd Party Libraries used:");
    int major, minor, patch;
    App->GetSDLVersion(major, minor, patch);
    IMGUI_BULLET(IMGUI_BLACK, "SDL", "%d.%d.%d", major, minor, patch);
    ImGui::BulletText("Glew 7.0");
    ImGui::BulletText("GPU Detect (2015)");
    ImGui::BulletText("imgui v1.85");
    ImGui::BulletText("MathGeoLib 1.5");
    ImGui::BulletText("Parson 1.2.1");

    ImGui::Separator();
    ImGui::TextWrapped("License:");
    ImGui::Spacing();
    ImGui::TextWrapped("MIT License");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextWrapped("Copyright (c) 2021 Guillem Alava & Sergi Colomer");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextWrapped("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

    ImGui::End();
}