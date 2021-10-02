#pragma once

#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
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
	// Frame variables
	PerfTimer pTimer;
	uint64 frameCount = 0;
	uint32 lastFrameMs = 0;
	float averageFps = 0.0f;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	float dt = 0.0f;

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
	float GetFps();
	float GetMs();
	void GetSDLVersion(int& major, int& minor, int& patch);
	void GetCPU(int& count, int& size);
	float GetRAM();
	void GetCaps(bool& threeD, bool& altiVec, bool& avx, bool& avx2, bool& mmx, bool& rdtsc, bool& sse, bool& sse2, bool& sse3, bool& sse41, bool& sse42);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};