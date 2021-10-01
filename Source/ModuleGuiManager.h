#ifndef __ModuleGuiManager_H__
#define __ModuleGuiManager_H__

#include "Module.h"
#include "Globals.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

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

public:
	bool console;
	bool config;
	bool about;
	bool demo;
};


#endif // __ModuleGuiManager_H__