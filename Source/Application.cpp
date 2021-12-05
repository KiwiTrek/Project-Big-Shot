#include "Application.h"
#include <shellapi.h>

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"
#include "ModuleCamera3D.h"
#include "ModuleImporter.h"
#include "ModuleGameObjects.h"
#include "ModuleResources.h"
#include "ModuleViewportBuffer.h"

Application::Application(ConsoleBuffer* _buff)
{
	window = new ModuleWindow(this);
	camera = new ModuleCamera3D(this);
	input = new ModuleInput(this);
	importer = new ModuleImporter(this);
	scene = new ModuleScene(this);
	gameObjects = new ModuleGameObjects(this);
	gui = new ModuleGuiManager(this);
	fileSystem = new ModuleFileSystem(this);
	renderer3D = new ModuleRenderer3D(this);
	resources = new ModuleResources(this);
	viewport = new ModuleViewportBuffer(this);

	AddModule(fileSystem);
	AddModule(resources);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(importer);

	AddModule(scene);
	AddModule(gameObjects);

	AddModule(viewport);
	AddModule(gui);
	AddModule(renderer3D);

	framesPerSecTime.Stop();
	frameCounter = 0;
	buff = _buff;
}

Application::~Application()
{
	std::vector<Module*>::reverse_iterator item = listModules.rbegin();
	while (item != listModules.rend())
	{
		delete (*item);
		++item;
	}
	listModules.clear();

	appName.clear();
	orgName.clear();

	window = nullptr;
	input = nullptr;
	scene = nullptr;
	renderer3D = nullptr;
	camera = nullptr;
	gui = nullptr;
	importer = nullptr;
	gameObjects = nullptr;
	buff->initBuff = nullptr;
	buff->initBuff2 = nullptr;
	buff = nullptr;
}

bool Application::Init()
{
	bool ret = true;

	std::vector<Module*>::iterator item = listModules.begin();
	while (item != listModules.end() && ret == true)
	{
		ret = (*item)->Init();
		++item;
	}

	LoadEngineConfig();

	if (gui != nullptr) gui->LogConsole(LOG("-------------- Application Start --------------"));

	item = listModules.begin();
	while (item != listModules.end() && ret == true)
	{
		ret = (*item)->Start();
		++item;
	}

	PERF_START(framesPerSecTime);
	return ret;
}

void Application::LoadEngineConfig()
{
	char* buffer = nullptr;
	uint bytesFile = fileSystem->Load("engineConfig.cfg", &buffer);

	if (bytesFile)
	{
		rapidjson::Document document;
		if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
		{
			if (gui != nullptr) gui->LogConsole(LOG("Error loading engine config"));
		}
		else
		{
			const rapidjson::Value config = document.GetObjectJSON();

			for (size_t i = 0; i < listModules.size(); i++)
			{
				listModules[i]->OnLoad(config);
			}

			if (gui != nullptr) gui->LogConsole(LOG("Engine config loaded"));
		}
	}
	RELEASE_ARRAY(buffer);
}

void Application::SaveEngineConfig()
{
	rapidjson::StringBuffer sb;
	JSONWriter writer(sb);

	writer.StartObject();
	for (size_t i = 0; i < listModules.size(); i++)
	{
		listModules[i]->OnSave(writer);
	}
	writer.EndObject();

	if (fileSystem->Save("engineConfig.cfg", sb.GetString(), strlen(sb.GetString()), false))
	{
		if (gui != nullptr) gui->LogConsole(LOG("Engine configuration saved."));
	}
	else
	{
		if (gui != nullptr) gui->LogConsole(LOG("Engine configuration not saved."));
	}
}

void Application::PrepareUpdate()
{
	frameCounter++;

	// Calculate dt
	dt = frameTime.ReadSec();

	// Start the timer after read because we want to know how much time it took from the last frame to the new one
	PERF_START(frameTime);
	PERF_START(pTimer);
}

void Application::FinishUpdate()
{
	// Framerate calculations------------------------------------------
	if (framesPerSecTime.Read() >= 1000)
	{
		averageFps = (float)frameCounter / framesPerSecTime.ReadSec();
		PERF_START(framesPerSecTime);
		frameCounter = 0;
	}

	lastFrameMsFloat = (float)pTimer.ReadMs();
	lastFrameMs = (uint32)lastFrameMsFloat;

	// Use SDL_Delay to make sure you get your capped framerate
	if (float(1000 / fpsLimit) > lastFrameMsFloat)
	{
		SDL_Delay((uint32)floor((double)(1000.0f / (float)fpsLimit) - lastFrameMsFloat));
		lastFrameMsFloat = (float)pTimer.ReadMs();
		PERF_START(pTimer);
	}

	if (gui->config != nullptr) gui->config->UpdateHistogram();
}

UpdateStatus Application::Update()
{
	// Call PreUpdate, Update and PostUpdate on all modules
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;
	PrepareUpdate();

	std::vector<Module*>::iterator item = listModules.begin();
	while (item != listModules.end() && ret == UpdateStatus::UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate();
		++item;
	}

	item = listModules.begin();
	while (item != listModules.end() && ret == UpdateStatus::UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		++item;
	}

	item = listModules.begin();
	while (item != listModules.end() && ret == UpdateStatus::UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate();
		++item;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::vector<Module*>::reverse_iterator item = listModules.rbegin();
	while (item != listModules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		++item;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	listModules.push_back(mod);
}

void Application::RequestBrowser(const char* link)
{
	ShellExecute(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
}

void Application::SetAppName(std::string _name)
{
	if (!_name.empty())
	{
		appName = _name;
		window->SetTitle(_name.c_str());
	}
}

std::string Application::GetAppName()
{
	return appName;
}

void Application::SetOrgName(std::string _name)
{
	if (!_name.empty()) orgName = _name;
}

std::string Application::GetOrgName()
{
	return orgName;
}

void Application::SetFpsLimit(int _limit)
{
	fpsLimit = _limit;
}

int Application::GetFpsLimit()
{
	return fpsLimit;
}

float Application::GetFps()
{
	return averageFps;
}

float Application::GetMs()
{
	return lastFrameMsFloat;
}

void Application::GetSDLVersion(int& major, int& minor, int& patch)
{
	SDL_version v;
	SDL_GetVersion(&v);
	major = v.major;
	minor = v.minor;
	patch = v.patch;
}


void Application::GetCPU(int& count, int& size)
{
	count = SDL_GetCPUCount();
	size = SDL_GetCPUCacheLineSize();
}

float Application::GetRAM()
{
	return (float)SDL_GetSystemRAM() / 1000.0f;
}

void Application::GetCaps(bool& threeD, bool& altiVec, bool& avx, bool& avx2, bool& mmx, bool& rdtsc, bool& sse, bool& sse2, bool& sse3, bool& sse41, bool& sse42)
{
	threeD = (bool)SDL_Has3DNow();
	altiVec = (bool)SDL_HasAltiVec();
	avx = (bool)SDL_HasAVX();
	avx2 = (bool)SDL_HasAVX2();
	mmx = (bool)SDL_HasMMX();
	rdtsc = (bool)SDL_HasRDTSC();
	sse = (bool)SDL_HasSSE();
	sse2 = (bool)SDL_HasSSE2();
	sse3 = (bool)SDL_HasSSE3();
	sse41 = (bool)SDL_HasSSE41();
	sse42 = (bool)SDL_HasSSE42();
}

void Application::GetGPU(uint& gpuVendor, uint& gpuDevice, char* gpuBrand, float& vramBudget, float& vramUsage, float& vramAvailable, float& vramReserved)
{
	std::wstring brand;
	uint64 videoMemBudget, videoMemCurrent, videoMemAvailable, videoMemReserved;
	if (getGraphicsDeviceInfo(&gpuVendor, &gpuDevice, &brand, &videoMemBudget, &videoMemCurrent, &videoMemAvailable, &videoMemReserved))
	{
		sprintf_s(gpuBrand, 250, "%S", brand.c_str());
		vramBudget = float(videoMemBudget) / (1024.f * 1024.f);
		vramUsage = float(videoMemCurrent) / (1024.f * 1024.f);
		vramAvailable = float(videoMemAvailable) / (1024.f * 1024.f);
		vramReserved = float(videoMemReserved) / (1024.f * 1024.f);
	}
}