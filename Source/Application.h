#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "Module.h"
#include "ModuleGuiManager.h"

class ModuleWindow;
class ModuleInput;
class ModuleScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleFileSystem;
class ModuleImporter;
class ModuleGameObjects;
class ModuleResources;
class ModuleViewportBuffer;

class Application
{
public:
	Application(ConsoleBuffer* _buff);
	~Application();

	bool Init();
	UpdateStatus Update();
	bool CleanUp();

	void SetAppName(std::string _name);
	std::string GetAppName();
	void SetOrgName(std::string _name);
	std::string GetOrgName();

	void LoadEngineConfig();
	void SaveEngineConfig();

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
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleScene* scene = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	ModuleFileSystem* fileSystem = nullptr;
	ModuleGuiManager* gui = nullptr;
	ModuleImporter* importer = nullptr;
	ModuleGameObjects* gameObjects = nullptr;
	ModuleResources* resources = nullptr;
	ModuleViewportBuffer* viewport = nullptr;

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