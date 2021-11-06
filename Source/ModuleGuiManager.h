#ifndef __MODULE_GUI_MANAGER_H__
#define __MODULE_GUI_MANAGER_H__

#include "Module.h"
#include "Panel.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "DeviceId.h"

#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelConfig.h"
#include "PanelHierarchy.h"
#include "PanelInspector.h"

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
};

#endif // !__MODULE_GUI_MANAGER_H__