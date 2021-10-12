#ifndef __MODULE_GUI_MANAGER_H__
#define __MODULE_GUI_MANAGER_H__

#include "Module.h"
#include "Panel.h"
#include "Globals.h"
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "DeviceId.h"

#include "PanelAbout.h"
#include "PanelConsole.h"
#include "PanelConfig.h"

class ModuleGuiManager : public Module
{
public:
	ModuleGuiManager(Application* app, bool start_enabled = true);
	~ModuleGuiManager();

	bool Start();
	update_status PreUpdate();
	update_status Update(float dt);
	update_status PostUpdate();
	bool CleanUp();

	void AddPanel(Panel* panel);

	update_status MenuBar();

	void LogConsole(const char* buff);

public:
	std::vector<Panel*> list_panels;

	PanelAbout* about;
	PanelConsole* console;
	PanelConfig* config;

	bool demo;
	ImGuiTextBuffer LogInputText;
	ImGuiTextBuffer LogConsoleText;
};

#endif // !__MODULE_GUI_MANAGER_H__