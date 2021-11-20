#include "ModuleGuiManager.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleGameObjects.h"

ModuleGuiManager::ModuleGuiManager(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "gui";
	// TODO: Check init buffers
	//LogConsoleText.appendf(App->buff->initBuff);
	//LogConsoleText.appendf(App->buff->initBuff2);

	about = new PanelAbout(App);
	console = new PanelConsole(App);
	config = new PanelConfig(App);
	hierarchy = new PanelHierarchy(App);
	inspector = new PanelInspector(App);
	sceneView = new PanelSceneView(App);

	AddPanel(about);
	AddPanel(console);
	AddPanel(config);
	AddPanel(hierarchy);
	AddPanel(inspector);
	//AddPanel(sceneView);
}

ModuleGuiManager::~ModuleGuiManager()
{
	std::vector<Panel*>::reverse_iterator item = listPanels.rbegin();

	while (item != listPanels.rend())
	{
		delete (*item);
		++item;
	}
	listPanels.clear();
}

bool ModuleGuiManager::Start()
{
	bool status = true;
	LOG_CONSOLE("Initializing Editor windows");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init("#version 460");

	SetupStyle();
	LogConsole(LOG("Loading Gui Manager"));
	demo = false;

	std::vector<Panel*>::iterator item = listPanels.begin();
	while (item != listPanels.end() && status)
	{
		status = (*item)->Start();
		++item;
	}

	return status;
}

UpdateStatus ModuleGuiManager::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);

	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();
	usingKeyboard = io.WantCaptureKeyboard;

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleGuiManager::Update(float dt)
{
	UpdateStatus status = UpdateStatus::UPDATE_CONTINUE;

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

	std::vector<Panel*>::iterator item = listPanels.begin();
	while (item != listPanels.end() && status == UpdateStatus::UPDATE_CONTINUE)
	{
		if ((*item)->active == true) status = (*item)->Update();
		++item;
	}

	if (demo) ImGui::ShowDemoWindow();

	return status;
}

UpdateStatus ModuleGuiManager::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backupWindow = SDL_GL_GetCurrentWindow();
		SDL_GLContext backupContext = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backupWindow, backupContext);
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleGuiManager::CleanUp()
{
	std::vector<Panel*>::reverse_iterator panel = listPanels.rbegin();
	while (panel != listPanels.rend())
	{
		(*panel)->CleanUp();
		panel++;
	}
	listPanels.clear();

	about = nullptr;
	console = nullptr;
	config = nullptr;
	hierarchy = nullptr;
	inspector = nullptr;
	sceneView = nullptr;
	LogInputText.clear();
	LogConsoleText.clear();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ModuleGuiManager::AddPanel(Panel* panel)
{
	listPanels.push_back(panel);
}

UpdateStatus ModuleGuiManager::MenuBar()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_STATE::KEY_DOWN)
	{
		config->active = !config->active;
	}

	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_STATE::KEY_DOWN)
	{
		hierarchy->active = !hierarchy->active;
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_STATE::KEY_DOWN)
	{
		inspector->active = !inspector->active;
	}

	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_STATE::KEY_DOWN)
	{
		console->active = !console->active;
	}

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ESC"))
			{
				return UpdateStatus::UPDATE_STOP;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Configuration", "F1", &config->active);
			ImGui::MenuItem("Hierarchy", "F2", &hierarchy->active);
			ImGui::MenuItem("Inspector", "F3", &inspector->active);
			ImGui::MenuItem("SceneView", nullptr, &sceneView->active);
			ImGui::MenuItem("Console", "F10", &console->active);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Empty"))
			{
				GameObject* g = new GameObject("Empty GameObject");
				App->gameObjects->AddGameobject(g);
				App->gameObjects->selectedGameObject = g;
			}

			if (ImGui::BeginMenu("GameObject"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					GameObject* c = new GameObject("Cube");
					c->CreatePrimitive(MeshTypes::Primitive_Cube);
					c->SetAxis(true);
					App->gameObjects->AddGameobject(c);
					App->gameObjects->selectedGameObject = c;

				}

				if (ImGui::MenuItem("Plane"))
				{
					GameObject* pl = new GameObject("Plane");
					pl->CreatePrimitive(MeshTypes::Primitive_Plane);
					pl->SetAxis(true);
					App->gameObjects->AddGameobject(pl);
					App->gameObjects->selectedGameObject = pl;
				}

				if (ImGui::MenuItem("Sphere"))
				{
					GameObject* s = new GameObject("Sphere");
					s->CreatePrimitive(MeshTypes::Primitive_Sphere);
					s->SetAxis(true);
					App->gameObjects->AddGameobject(s);
					App->gameObjects->selectedGameObject = s;
				}

				if (ImGui::MenuItem("Cylinder"))
				{
					GameObject* cyl = new GameObject("Cylinder");
					cyl->CreatePrimitive(MeshTypes::Primitive_Cylinder);
					cyl->SetAxis(true);
					App->gameObjects->AddGameobject(cyl);
					App->gameObjects->selectedGameObject = cyl;
				}

				if (ImGui::MenuItem("Pyramid"))
				{
					GameObject* p = new GameObject("Pyramid");
					p->CreatePrimitive(MeshTypes::Primitive_Pyramid);
					p->SetAxis(true);
					App->gameObjects->AddGameobject(p);
					App->gameObjects->selectedGameObject = p;
				}
				ImGui::EndMenu();
			}
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
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleGuiManager::LogConsole(const char* buff)
{
	LogConsoleText.appendf(buff);
	console->update = true;
}

const char* ModuleGuiManager::GetImGuiVersion()
{
	return IMGUI_VERSION;
}

bool ModuleGuiManager::GetInput(SDL_Event* event)
{
	return ImGui_ImplSDL2_ProcessEvent(event);
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
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f,1.00f,1.00f,0.50f);
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
	colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
	colors[ImGuiCol_DockingPreview] = IMGUI_LIGHT_GREEN;
	colors[ImGuiCol_DockingEmptyBg] = IMGUI_WHITE;
	colors[ImGuiCol_PlotLines] = IMGUI_PINK;
	colors[ImGuiCol_PlotLinesHovered] = IMGUI_YELLOW;
	colors[ImGuiCol_PlotHistogram] = IMGUI_PINK;
	colors[ImGuiCol_PlotHistogramHovered] = IMGUI_YELLOW;
	colors[ImGuiCol_TableHeaderBg] = IMGUI_PINK;
	colors[ImGuiCol_TableBorderStrong] = IMGUI_PINK;
	colors[ImGuiCol_TableBorderLight] = IMGUI_LIGHT_PINK;
	colors[ImGuiCol_TableRowBg] = IMGUI_BLACK;
	colors[ImGuiCol_TableRowBgAlt] = IMGUI_DARK_PURPLE;
	colors[ImGuiCol_DragDropTarget] = IMGUI_LIGHT_GREEN;
	colors[ImGuiCol_NavHighlight] = IMGUI_YELLOW;
	colors[ImGuiCol_NavWindowingHighlight] = IMGUI_GREY;
	colors[ImGuiCol_NavWindowingDimBg] = IMGUI_LIGHT_GREY;
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}