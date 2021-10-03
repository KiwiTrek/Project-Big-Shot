#include "Application.h"
#include <shellapi.h>

Application::Application(ConsoleBuffer* _buff)
{
	buff = _buff;
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGuiManager(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
	// Scenes
	AddModule(scene_intro);

	AddModule(gui);
	AddModule(renderer3D);
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

	PERF_START(pTimer);

	// Call Init() in all modules
	std::vector<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init();
		++item;
	}

	PERF_PEEK(pTimer);

	PERF_START(pTimer);

	// After all Init calls we call Start() in all modules
	if (gui != nullptr) gui->LogConsole(LOG("Application Start --------------"));
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		++item;
	}

	PERF_PEEK(pTimer);
	
	return ret;
}


// ---------------------------------------------
void Application::PrepareUpdate()
{
	frameCount++;
	lastSecFrameCount++;

	// Calculate the dt: differential time since last frame
	dt = frameTime.ReadSec();

	// Start the timer after read because we want to know how much time it took from the last frame to the new one
	PERF_START(frameTime);
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	// Framerate calculations------------------------------------------
	// To know how many frames have passed in the last second
	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		prevLastSecFrameCount = lastSecFrameCount;
		lastSecFrameCount = 0;
	}

	// Amount of seconds since startup
	float secondsSinceStartup = 0.0f;
	secondsSinceStartup = startupTime.ReadSec();

	// Amount of time since game start (use a low resolution timer)
	lastFrameMs = pTimer.ReadMs(); // Time from the prepare update until now (whole update method)

	// Average FPS for the whole game life (since start)
	averageFps = float(frameCount) / startupTime.ReadSec();

	// Amount of frames during the last update
	uint32 framesOnLastUpdate = 0;
	framesOnLastUpdate = prevLastSecFrameCount;

	// Use SDL_Delay to make sure you get your capped framerate
	PERF_START(pTimer);
	if ((1000 / fpsLimit) > lastFrameMs)
	{
		SDL_Delay((1000 / fpsLimit) - lastFrameMs);
	}
	gui->UpdateHistogram();
	// Measure accurately the amount of time SDL_Delay() actually waits compared to what was expected
	PERF_PEEK(pTimer);
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::vector<Module*>::iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate();
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update();
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
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
	return lastFrameMs;
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
	threeD = SDL_Has3DNow();
	altiVec = SDL_HasAltiVec();
	avx = SDL_HasAVX();
	avx2 = SDL_HasAVX2();
	mmx = SDL_HasMMX();
	rdtsc = SDL_HasRDTSC();
	sse = SDL_HasSSE();
	sse2 = SDL_HasSSE2();
	sse3 = SDL_HasSSE3();
	sse41 = SDL_HasSSE41();
	sse42 = SDL_HasSSE42();
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
//			ReadConfiguration(config.GetSection("App"));
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
//void Application::SavePrefs() const
//{
//	ConfigJSON config;
//
//	SaveConfiguration(config.AddSection("App"));
//
//	for (list<Module*>::const_iterator it = modules.begin(); it != modules.end(); ++it)
//		(*it)->Save(&config.AddSection((*it)->GetName()));
//
//	char* buf;
//	uint size = config.Save(&buf, "Saved preferences for Edu Engine");
//	if (App->fs->Save(SETTINGS_FOLDER "config.json", buf, size) > 0)
//		LOG("Saved Engine Preferences");
//	RELEASE_ARRAY(buf);
//}
//
//void Application::ReadConfiguration(const ConfigJSON& config)
//{
//	appName = config.GetString("Name", "Edu Engine");
//	organizationName = config.GetString("Organization", "UPC CITM");
//	SetFramerateLimit(config.GetInt("MaxFramerate", 0));
//}
//
//void Application::SaveConfiguration(ConfigJSON& config) const
//{
//
//}