#include "PanelConfig.h"
#include "Application.h"
#include "ModuleGuiManager.h"

PanelConfig::PanelConfig(Application* app, bool start_enabled) : Panel(app, start_enabled), fpsHist(100), msHist(100)
{
    name = "console";
}

PanelConfig::~PanelConfig()
{}

update_status PanelConfig::Update()
{
    ImGui::Begin("Configuration");
    if (ImGui::BeginMenu("Options"))
    {
        //TODO: JSON Parser
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
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
        flags &= ~ImGuiInputTextFlags_ReadOnly;

        std::string appName = App->GetAppName();
        if (ImGui::InputText("Project Name", &appName, flags))
        {
            App->SetAppName(appName);
        }

        std::string orgName = App->GetOrgName();
        if (ImGui::InputText("Organization", &orgName, flags))
        {
            App->SetOrgName(orgName);
        }
        ImGui::Separator();

        int maxFPS = App->GetFpsLimit();
        if (ImGui::SliderInt("Max FPS", &maxFPS, 1, 120))
        {
            App->SetFpsLimit(maxFPS);
        }

        std::string Title = "Framerate: " + std::to_string(App->GetFps());
        ImGui::PlotHistogram("##framerate", &fpsHist[0], fpsHist.size(), 0, Title.c_str(), 0.0f, 120.0f, ImVec2(ImGui::CalcItemWidth(), 100.0f));
        Title = "Milliseconds: " + std::to_string(App->GetMs());
        ImGui::PlotHistogram("##milliseconds", &msHist[0], msHist.size(), 0, Title.c_str(), 0.0f, 120.0f, ImVec2(ImGui::CalcItemWidth(), 100.0f));
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
        IMGUI_PRINT(IMGUI_BLUE, "GPU: ", "VendorId: %d - DeviceId: %d", vendorId, deviceId);
        IMGUI_PRINT(IMGUI_BLUE, "Brand: ", brand);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Budget: ", "%.1f Mb", videoMemBudget);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Usage: ", "%.1f Mb", videoMemCurrent);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Available: ", "%.1f Mb", videoMemAvailable);
        IMGUI_PRINT(IMGUI_BLUE, "VRAM Reserved: ", "%.1f Mb", videoMemReserved);

    }

    if (ImGui::CollapsingHeader("Render"))
    {
        IMGUI_PRINT(IMGUI_BLUE, "Video Driver:", "%s", App->renderer3D->GetVideoDriver());

        bool vSync = App->renderer3D->GetVSync();
        if (ImGui::Checkbox("VSync", &vSync))
        {
            App->renderer3D->SetVSync(vSync);
        }
        ImGui::SameLine();
        bool wireframe = App->renderer3D->IsWireframe();
        if (ImGui::Checkbox("Wireframe", &wireframe))
        {
            App->renderer3D->ToggleWireframe();
        }

        bool depthTest = App->renderer3D->IsDepthTest();
        if (ImGui::Checkbox("Depth Test", &depthTest))
        {
            App->renderer3D->ToggleDepthTest();
        }
        ImGui::SameLine();
        bool cullFace = App->renderer3D->IsCullFace();
        if (ImGui::Checkbox("Cull Face", &cullFace))
        {
            App->renderer3D->ToggleCullFace();
        }

        bool lighting = App->renderer3D->IsLighting();
        if (ImGui::Checkbox("Lighting", &lighting))
        {
            App->renderer3D->ToggleLighting();
        }
        ImGui::SameLine();
        bool colorMaterial = App->renderer3D->IsColorMaterial();
        if (ImGui::Checkbox("Color Material", &colorMaterial))
        {
            App->renderer3D->ToggleColorMaterial();
        }

        bool texture2D = App->renderer3D->IsTexture2D();
        if (ImGui::Checkbox("Texture 2D", &texture2D))
        {
            App->renderer3D->ToggleTexture2D();
        }
        ImGui::SameLine();
        bool faceNormals = App->renderer3D->IsFaceNormals();
        if (ImGui::Checkbox("Face Normals", &faceNormals))
        {
            App->renderer3D->ToggleFaceNormals();
        }

        bool vertexNormals = App->renderer3D->IsVertexNormals();
        if (ImGui::Checkbox("Vertex Normals", &vertexNormals))
        {
            App->renderer3D->ToggleVertexNormals();
        }
    }

    if (ImGui::CollapsingHeader("Input"))
    {
        int mouseX, mouseY;
        mouseX = App->input->GetMouseX();
        mouseY = App->input->GetMouseY();
        IMGUI_PRINT(IMGUI_BLUE, "Mouse Position: ", "%d,%d", mouseX, mouseY);
        int mousewheel = App->input->GetMouseZ();
        IMGUI_PRINT(IMGUI_BLUE, "Mousewheel: ", "%d", mousewheel);
        int mouseMotionX, mouseMotionY;
        mouseMotionX = App->input->GetMouseXMotion();
        mouseMotionY = App->input->GetMouseYMotion();
        IMGUI_PRINT(IMGUI_BLUE, "Mouse Motion: ", "%d,%d", mouseMotionX, mouseMotionY);
        ImGui::Separator();
        if (ImGui::Button("Clear", ImVec2(ImGui::CalcItemWidth(), 20)))
        {
            App->gui->LogInputText.clear();
        }
        ImGui::BeginChild("Input Log");
        ImGui::TextUnformatted(App->gui->LogInputText.begin());
        ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
    ImGui::End();
    return update_status::UPDATE_CONTINUE;
}

void PanelConfig::UpdateHistogram()
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