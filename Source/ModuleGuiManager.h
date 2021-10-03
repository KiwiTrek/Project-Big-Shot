#ifndef __ModuleGuiManager_H__
#define __ModuleGuiManager_H__

#include "Module.h"
#include "Globals.h"
#include <vector>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "DeviceId.h"

class ModuleGuiManager : public Module
{
public:
	ModuleGuiManager(Application* app, bool start_enabled = true);
	~ModuleGuiManager();

	bool Start();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	update_status MenuBar();
	void Config();
	void UpdateHistogram();

	void Console();
	void LogConsole(const char* buff);
	void About();

public:
	bool console;
	bool config;
	bool about;
	bool demo;
	std::vector<float> fpsHist;
	std::vector<float> msHist;
	ImGuiTextBuffer LogInputText;
	ImGuiTextBuffer LogConsoleText;
};

#endif // __ModuleGuiManager_H__