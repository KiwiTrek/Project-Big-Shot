#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Timer.h"
#include "PerfTimer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGuiManager.h"
#include "ModuleImporter.h"
#include "ModuleGameObjects.h"

class Application
{
public:
	Application(ConsoleBuffer* _buff);
	~Application();

	bool Init();
	UpdateStatus Update();
	bool CleanUp();

	// TODO: Load/Save config
	//void LoadConfig();
	//void SaveConfig() const;

	void SetAppName(std::string _name);
	std::string GetAppName();
	void SetOrgName(std::string _name);
	std::string GetOrgName();

	void SetFpsLimit(int _limit);
	int GetFpsLimit();
	float GetFps();
	float GetMs();

	void RequestBrowser(const char* link);

	void GetSDLVersion(int& major, int& minor, int& patch);
	void GetCPU(int& count, int& size);
	float GetRAM();
	void GetCaps(bool& threeD, bool& altiVec, bool& avx, bool& avx2, bool& mmx, bool& rdtsc, bool& sse, bool& sse2, bool& sse3, bool& sse41, bool& sse42);
	void GetGPU(uint& gpuVendor, uint& gpuDevice, char* gpuBrand, float& vramBudget, float& vramUsage, float& vramAvailable, float& vramReserved);

private:
	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGuiManager* gui;
	ModuleImporter* importer;
	ModuleGameObjects* gameObjects;

	ConsoleBuffer* buff;

private:
	std::string appName = "Project Big Shot";
	std::string orgName = "UPC CITM";

	std::vector<Module*> listModules;

	// Frame variables
	int fpsLimit = 60;
	PerfTimer pTimer;
	uint32 lastFrameMs = 0;
	float lastFrameMsFloat = 0.0f;
	float averageFps = 0.0f;

	Timer frameTime;
	int frameCounter;
	Timer framesPerSecTime;
	float dt = 0.0f;
};

#endif // !__APPLICATION_H__