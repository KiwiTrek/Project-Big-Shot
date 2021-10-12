#include "Application.h"
#include <shellapi.h>

Application::Application(ConsoleBuffer* _buff)
{
	buff = _buff;
	gui = new ModuleGuiManager(this);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	sceneIntro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	fileSystem = new ModuleFileSystem(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(fileSystem);
	
	// Scenes
	AddModule(sceneIntro);

	AddModule(gui);
	AddModule(renderer3D);

	framesPerSecTime.Stop();
	frameCounter = 0;
}

Application::~Application()
{
	std::vector<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend())
	{
		delete *item;
		++item;
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	std::vector<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init();
		++item;
	}

	// After all Init calls we call Start() in all modules
	if (gui != nullptr) gui->LogConsole(LOG("Application Start --------------"));
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		++item;
	}

	PERF_START(framesPerSecTime);
	
	return ret;
}


// ---------------------------------------------
void Application::PrepareUpdate()
{
	frameCounter++;
	// Calculate the dt: differential time since last frame
	dt = frameTime.ReadSec();

	// Start the timer after read because we want to know how much time it took from the last frame to the new one
	PERF_START(frameTime);
	PERF_START(pTimer);
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	// Framerate calculations------------------------------------------
	if (framesPerSecTime.Read() >= 1000)
	{
		//LOG("%d", frameCounter);
		//LOG("%d", framesPerSecTime.Read());
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
		//PERF_PEEK(pTimer);
		PERF_START(pTimer);
	}

	if (gui->config != nullptr) gui->config->UpdateHistogram();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::vector<Module*>::iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == update_status::UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate();
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == update_status::UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == update_status::UPDATE_CONTINUE)
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
	std::vector<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend() && ret == true)
	{
		ret = (*item)->CleanUp();
		++item;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
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
const char* Application::GetAppName()
{
	return appName.c_str();
}

void Application::SetOrgName(std::string _name)
{
	if (!_name.empty())
	{
		orgName = _name;
	}
}
const char* Application::GetOrgName()
{
	return orgName.c_str();
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

//uint Application::GetFramerateLimit() const
//{
//	if (capped_ms > 0)
//		return (uint)((1.0f / (float)capped_ms) * 1000.0f);
//	else
//		return 0;
//}
//
//void Application::SetFramerateLimit(uint max_framerate)
//{
//	if (max_framerate > 0)
//		capped_ms = 1000 / max_framerate;
//	else
//		capped_ms = 0;
//}
//
//void Application::LoadConfig()
//{
//	char* buffer = "/Engine/config.json";
//
//	if (buffer != nullptr)
//	{
//		ConfigJSON config((const char*)buffer);
//
//		if (config.IsValid() == true)
//		{
//			LOG("Loading Engine Preferences");
//
//			SetAppName(config.GetSection("App").GetString("Name", "Project Big Shot"));
//			SetOrgName(config.GetSection("App").GetString("Organization", "UPC CITM"));
//			SetFramerateLimit(config.GetSection("App").GetInt("MaxFramerate", 0));
//
//			ConfigJSON section;
//			std::vector<Module*>::iterator item = list_modules.begin();
//
//			while (item != list_modules.end())
//			{
//				section = config.GetSection((*item)->GetName());
//				//if (section.IsValid())
//				(*item)->Load(&section);
//			}
//		}
//		else
//			LOG("Cannot load Engine Preferences: Invalid format");
//
//		RELEASE_ARRAY(buffer);
//	}
//}
//
//void Application::SaveConfig() const
//{
//	ConfigJSON config;
//
//	config.AddSection("App").AddString("Name", appName.c_str());
//	config.AddSection("App").AddString("Organization", orgName.c_str());
//	config.AddSection("App").AddInt("MaxFramerate", GetFramerateLimit());
//
//	std::vector<Module*>::reverse_iterator item = list_modules.rbegin();
//
//	while (item != list_modules.rend())
//	{
//		(*item)->Save(&config.AddSection((*item)->GetName()));
//	}
//
//	char* buf;
//	uint size = config.Save(&buf, "Saved preferences for Project Big Shot");
//	if (App->fs->Save("/Engine/config.json", buf, size) > 0)
//		LOG("Saved Engine Preferences");
//	RELEASE_ARRAY(buf);
//}