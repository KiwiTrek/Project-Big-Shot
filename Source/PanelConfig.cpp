#include "PanelConfig.h"
#include "Application.h"
#include "ModuleGuiManager.h"

#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleImporter.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"

PanelConfig::PanelConfig(Application* app, bool startEnabled) : Panel(app, startEnabled), fpsHist(100), msHist(100)
{
	name = "Configuration";
}

PanelConfig::~PanelConfig()
{}

bool PanelConfig::Start()
{
	App->GetSDLVersion(sdlMajor, sdlMinor, sdlPatch);
	App->GetCPU(cpuCount, cpuSize);
	ramSize = App->GetRAM();
	App->GetCaps(threeD, altiVec, avx, avx2, mmx, rdtsc, sse, sse2, sse3, sse41, sse42);
	App->GetGPU(gpuVendorId, gpuDeviceId, gpuBrand, videoMemBudget, videoMemCurrent, videoMemAvailable, videoMemReserved);
	App->importer->GetAssimpVersion(assimpMajor, assimpMinor, assimpPatch);
	glewVersion = App->renderer3D->GetGlewVersion();
	glVersion = App->renderer3D->GetOpenGLVersion();
	imguiVersion = App->gui->GetImGuiVersion();
	return true;
}

UpdateStatus PanelConfig::Update()
{
	ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;

	ImGui::Begin(name.c_str(), &active);

	if (ImGui::CollapsingHeader("Application"))
	{
		std::string appName = App->GetAppName();
		if (ImGui::InputText("Project Name", &appName, flags))	App->SetAppName(appName);

		std::string orgName = App->GetOrgName();
		if (ImGui::InputText("Organization", &orgName, flags)) App->SetOrgName(orgName);

		ImGui::Separator();

		int maxFPS = App->GetFpsLimit();
		if (ImGui::SliderInt("Max FPS", &maxFPS, 1, 120)) App->SetFpsLimit(maxFPS);

		std::string Title = "Framerate: " + std::to_string(App->GetFps());
		ImGui::PlotHistogram("##framerate", &fpsHist[0], fpsHist.size(), 0, Title.c_str(), 0.0f, 120.0f, ImVec2(ImGui::CalcItemWidth(), 100.0f));
		Title = "Milliseconds: " + std::to_string(App->GetMs());
		ImGui::PlotHistogram("##milliseconds", &msHist[0], msHist.size(), 0, Title.c_str(), 0.0f, 120.0f, ImVec2(ImGui::CalcItemWidth(), 100.0f));
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		float b = App->window->GetBrightness();
		if (ImGui::SliderFloat("Brightness", &b, 0.05f, 1.0f)) App->window->SetBrightness(b);

		int w = App->window->GetWidth();
		int h = App->window->GetHeight();
		int maxW, maxH;
		App->window->GetMaxWindow(maxW, maxH);
		if (ImGui::InputInt("Width", &w, 1, 25, flags))
		{
			if (w < 320)
			{
				w = 320;
			}
			else if (w > maxW)
			{
				w = maxW;
			}

			App->window->SetWidth(w);
		}
		if (ImGui::InputInt("Height", &h, 1, 25, flags))
		{
			if (h < 200)
			{
				h = 200;
			}
			else if (h > maxH)
			{
				h = maxH;
			}

			App->window->SetHeight(h);
		}

		ImGui::Text("Refresh rate:");
		ImGui::SameLine();
		ImGui::TextColored(IMGUI_YELLOW, "%u", App->window->GetRefreshRate());

		ImGui::Separator();

		bool fullscreen = App->window->IsFullscreen();
		if (ImGui::Checkbox("Fullscreen", &fullscreen))  App->window->SetFullscreen(fullscreen);

		bool fullscreenDesktop = App->window->IsFullscreenDesktop();
		if (ImGui::Checkbox("Fullscreen Desktop", &fullscreenDesktop)) App->window->SetFullscreenDesktop(fullscreenDesktop);

		bool resizable = App->window->IsResizable();
		if (ImGui::Checkbox("Resizable", &resizable)) App->window->SetResizable(resizable);

		bool borderless = App->window->IsBorderless();
		if (ImGui::Checkbox("Borderless", &borderless)) App->window->SetBorderless(borderless);
	}

	if (ImGui::CollapsingHeader("Editor Camera"))
	{
		ImGui::SliderFloat("Speed", &App->camera->cameraSpeed, 1.0f, 120.0f);
		ImGui::SliderFloat("Sensitivity", &App->camera->cameraSensitivity, 0.05f, 0.5f);
		if (ImGui::SliderFloat("Vertical FOV", &App->camera->verticalFOV, 0.1f, 180.0f))
		{
			App->camera->projectionIsDirty = true;
		}
		if (ImGui::InputFloat("Near Plane", &App->camera->nearPlaneDistance, 1.0f, 10.0f))
		{
			if (App->camera->nearPlaneDistance <= 0.05f)
			{
				App->camera->nearPlaneDistance = 0.05f;
			}
			else if (App->camera->nearPlaneDistance > 25.0f)
			{
				App->camera->nearPlaneDistance = 25.0f;
			}
			App->camera->projectionIsDirty = true;
		}
		if (ImGui::SliderFloat("Far Plane", &App->camera->farPlaneDistance, 5.0f, 500.0f))
		{
			App->camera->projectionIsDirty = true;
		}
	}

	if (ImGui::CollapsingHeader("Hardware & Software"))
	{
		ImGui::SetWindowFontScale(1.2f);
		ImGui::TextColored(IMGUI_YELLOW, "Hardware:");
		ImGui::SetWindowFontScale(1);
		IMGUI_PRINT(IMGUI_YELLOW, "CPUs:", "%d (%dKb)", cpuCount, cpuSize);
		IMGUI_PRINT(IMGUI_YELLOW, "RAM:", "%.2fGb", ramSize);
		ImGui::Spacing();
		IMGUI_PRINT(IMGUI_YELLOW, "Caps:", "%s%s%s%s%s%s", threeD ? "3DNow, " : "", altiVec ? "AltiVec, " : "", avx ? "AVX, " : "", avx2 ? "AVX2, " : "", mmx ? "MMX, " : "", rdtsc ? "RDTSC, " : "");
		IMGUI_PRINT(IMGUI_YELLOW, "", "%s%s%s%s%s", sse ? "SSE, " : "", sse2 ? "SSE2, " : "", sse3 ? "SSE3, " : "", sse41 ? "SSE41, " : "", sse42 ? "SSE42" : "");
		ImGui::Spacing();
		IMGUI_PRINT(IMGUI_YELLOW, "GPU: ", "VendorId: %d - DeviceId: %d", gpuVendorId, gpuDeviceId);
		IMGUI_PRINT(IMGUI_YELLOW, "Brand: ", gpuBrand);
		IMGUI_PRINT(IMGUI_YELLOW, "VRAM Budget: ", "%.1f Mb", videoMemBudget);
		IMGUI_PRINT(IMGUI_YELLOW, "VRAM Usage: ", "%.1f Mb", videoMemCurrent);
		IMGUI_PRINT(IMGUI_YELLOW, "VRAM Available: ", "%.1f Mb", videoMemAvailable);
		IMGUI_PRINT(IMGUI_YELLOW, "VRAM Reserved: ", "%.1f Mb", videoMemReserved);

		ImGui::Separator();

		ImGui::SetWindowFontScale(1.2f);
		ImGui::TextColored(IMGUI_YELLOW, "Software (3rd party libraries):");
		ImGui::SetWindowFontScale(1);
		IMGUI_BULLET(IMGUI_YELLOW, "SDL", "%d.%d.%d", sdlMajor, sdlMinor, sdlPatch);
		IMGUI_BULLET(IMGUI_YELLOW, "OpenGL", "%s", glewVersion.c_str());
		IMGUI_BULLET(IMGUI_YELLOW, "Glew", "%s", glewVersion.c_str());
		ImGui::BulletText("GPU Detect (2015)");
		IMGUI_BULLET(IMGUI_YELLOW, "imgui", "%s", imguiVersion.c_str());
		IMGUI_BULLET(IMGUI_YELLOW, "MathGeoLib", "1.5");
		IMGUI_BULLET(IMGUI_YELLOW, "Assimp", "%d.%d.%d", assimpMajor, assimpMinor, assimpPatch);
		IMGUI_BULLET(IMGUI_YELLOW, "DevIL", "1.8.0");
		IMGUI_BULLET(IMGUI_YELLOW, "PhysFS", "v3.0.2");
	}

	if (ImGui::CollapsingHeader("Render"))
	{
		IMGUI_PRINT(IMGUI_YELLOW, "Video Driver:", "%s", App->renderer3D->GetVideoDriver());

		bool vSync = App->renderer3D->GetVSync();
		if (ImGui::Checkbox("VSync", &vSync)) App->renderer3D->SetVSync(vSync);
		ImGui::SameLine();
		bool wireframe = App->renderer3D->IsWireframe();
		if (ImGui::Checkbox("Wireframe", &wireframe)) App->renderer3D->ToggleWireframe();

		bool depthTest = App->renderer3D->IsDepthTest();
		if (ImGui::Checkbox("Depth Test", &depthTest)) App->renderer3D->ToggleDepthTest();
		ImGui::SameLine();
		bool cullFace = App->renderer3D->IsCullFace();
		if (ImGui::Checkbox("Cull Face", &cullFace)) App->renderer3D->ToggleCullFace();

		bool lighting = App->renderer3D->IsLighting();
		if (ImGui::Checkbox("Lighting", &lighting)) App->renderer3D->ToggleLighting();
		ImGui::SameLine();
		bool axis = App->renderer3D->IsAxis();
		if (ImGui::Checkbox("Show Axis", &axis)) App->renderer3D->ToggleAxis();

		bool colorMaterial = App->renderer3D->IsColorMaterial();
		if (ImGui::Checkbox("Color Material", &colorMaterial)) App->renderer3D->ToggleColorMaterial();
		ImGui::SameLine();
		bool texture2D = App->renderer3D->IsTexture2D();
		if (ImGui::Checkbox("Texture 2D", &texture2D)) App->renderer3D->ToggleTexture2D();

		ImGui::Checkbox("Draw Mouse Raycast", &App->scene->drawMouse);
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		int mouseX, mouseY;
		mouseX = App->input->GetMouseX();
		mouseY = App->input->GetMouseY();
		IMGUI_PRINT(IMGUI_YELLOW, "Mouse Position: ", "%d,%d", mouseX, mouseY);
		IMGUI_PRINT(IMGUI_YELLOW, "Mouse Pos. Scene: ", "%d,%d", (int)App->gui->mouseScenePosition.x, (int)App->gui->mouseScenePosition.y);
		int mousewheel = App->input->GetMouseZ();
		IMGUI_PRINT(IMGUI_YELLOW, "Mousewheel: ", "%d", mousewheel);
		int mouseMotionX, mouseMotionY;
		mouseMotionX = App->input->GetMouseXMotion();
		mouseMotionY = App->input->GetMouseYMotion();
		IMGUI_PRINT(IMGUI_YELLOW, "Mouse Motion: ", "%d,%d", mouseMotionX, mouseMotionY);

		ImGui::Separator();

		if (ImGui::Button("Clear", ImVec2(ImGui::CalcItemWidth(), 20))) App->gui->LogInputText.clear();

		ImGui::BeginChild("Input Log");
		ImGui::TextUnformatted(App->gui->LogInputText.begin());
		if (update)
		{
			ImGui::SetScrollHereY(1.0f);
			update = false;
		}
		ImGui::EndChild();
	}

	if (ImGui::BeginMenu("Options"))
	{
	    if (ImGui::MenuItem("Load"))
	    {
			App->LoadEngineConfig();
	    }
		if (ImGui::MenuItem("Save"))
		{
			App->SaveEngineConfig();
		}

	    ImGui::EndMenu();
	}

	ImGui::End();
	return UpdateStatus::UPDATE_CONTINUE;
}

bool PanelConfig::CleanUp()
{
	fpsHist.clear();
	msHist.clear();

	glVersion.clear();
	glewVersion.clear();
	imguiVersion.clear();

	return true;
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
	{
		++count;
	}

	fpsHist[count - 1] = App->GetFps();
	msHist[count - 1] = App->GetMs();
}