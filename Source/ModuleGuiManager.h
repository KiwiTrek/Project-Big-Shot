#ifndef __MODULE_GUI_MANAGER_H__
#define __MODULE_GUI_MANAGER_H__

#include "Module.h"
#include "Panel.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "DeviceId.h"

#include "Panel.h"
#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelConfig.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"
#include "PanelSceneView.h"

class Grid
{
public:
	Grid();
	Grid(float x, float y, float z, float d);

	void Render();
public:
	vec3 normal;
	float constant;
};

class ModuleGuiManager : public Module
{
public:
	ModuleGuiManager(Application* app, bool startEnabled = true);
	~ModuleGuiManager();

	bool Start();
	UpdateStatus PreUpdate();
	UpdateStatus Update(float dt);
	UpdateStatus PostUpdate();
	bool CleanUp();

	void AddPanel(Panel* panel);
	void SetupStyle();
	UpdateStatus MenuBar();
	void LogConsole(const char* buff);
	const char* GetImGuiVersion();
	bool GetInput(SDL_Event* event);

public:
	bool demo;
	bool usingKeyboard;
	ImGuiTextBuffer LogInputText;
	ImGuiTextBuffer LogConsoleText;

	std::vector<Panel*> listPanels;

	PanelAbout* about;
	PanelConsole* console;
	PanelConfig* config;
	PanelHierarchy* hierarchy;
	PanelInspector* inspector;
	PanelSceneView* sceneView;

	Grid grid;
};

#endif // !__MODULE_GUI_MANAGER_H__