#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGuiManager.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGuiManager* gui;

private:
	std::string appName = "Project Big Shot";
	std::string orgName = "UPC CITM";
	
	int fpsLimit = 60;

	std::vector<Module*> list_modules;
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(const char* link);

	void SetAppName(std::string _name);
	const char* GetAppName();
	void SetOrgName(std::string _name);
	const char* GetOrgName();
	void SetFpsLimit(int _limit);
	int GetFpsLimit();


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};