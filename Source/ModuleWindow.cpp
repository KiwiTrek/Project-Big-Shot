#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "window";
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Init SDL window & surface"));
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError()));
		ret = false;
	}
	else
	{
		//Create window
		int width = screenWidth * SCREEN_SIZE;
		int height = screenHeight * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreenDesktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->GetAppName(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			if (App->gui != nullptr) App->gui->LogConsole(LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError()));
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Destroying SDL window and quitting all SDL systems"));

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

float ModuleWindow::GetBrightness()
{
	return SDL_GetWindowBrightness(window);
}

void ModuleWindow::SetBrightness(float b)
{
	SDL_SetWindowBrightness(window, b);
}

int ModuleWindow::GetWidth()
{
	return screenWidth;
}

void ModuleWindow::SetWidth(int w)
{
	screenWidth = w;
	SDL_SetWindowSize(window,screenWidth,screenHeight);
}

int ModuleWindow::GetHeight()
{
	return screenHeight;
}

void ModuleWindow::SetHeight(int h)
{
	screenHeight = h;
	SDL_SetWindowSize(window, screenWidth, screenHeight);
}

void ModuleWindow::GetMaxWindow(int& w, int& h)
{
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError()));
	}
	else
	{
		w = dm.w;
		h = dm.h;
	}
}

uint ModuleWindow::GetRefreshRate()
{
	uint ret = 0;

	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError()));
	}
	else
	{
		ret = dm.refresh_rate;
	}
	return ret;
}

bool ModuleWindow::IsFullscreen()
{
	return fullscreen;
}

void ModuleWindow::SetFullscreen(bool f)
{
	if (f != fullscreen)
	{
		fullscreen = f;
		if (fullscreen == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0)
			{
				if (App->gui != nullptr) App->gui->LogConsole(LOG("Could not switch to fullscreen: %s\n", SDL_GetError()));
			}
			fullscreenDesktop = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
			{
				if (App->gui != nullptr) App->gui->LogConsole(LOG("Could not switch to windowed: %s\n", SDL_GetError()));
			}
		}
	}
}

bool ModuleWindow::IsResizable()
{
	return resizable;
}

void ModuleWindow::SetResizable(bool r)
{
	//TODO: save and load this value
	resizable = r;
}

bool ModuleWindow::IsBorderless()
{
	return borderless;
}

void ModuleWindow::SetBorderless(bool b)
{
	if (b != borderless && fullscreen == false && fullscreenDesktop == false)
	{
		borderless = b;
		SDL_SetWindowBordered(window, (SDL_bool)!borderless);
	}
}

bool ModuleWindow::IsFullscreenDesktop()
{
	return fullscreenDesktop;
}

void ModuleWindow::SetFullscreenDesktop(bool fd)
{
	if (fd != fullscreenDesktop)
	{
		fullscreenDesktop = fd;
		if (fullscreenDesktop == true)
		{
			if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
			{
				if (App->gui != nullptr) App->gui->LogConsole(LOG("Could not switch to fullscreen desktop: %s\n", SDL_GetError()));
			}
			fullscreen = false;
		}
		else
		{
			if (SDL_SetWindowFullscreen(window, 0) != 0)
			{
				if (App->gui != nullptr) App->gui->LogConsole(LOG("Could not switch to windowed: %s\n", SDL_GetError()));
			}
		}
	}
}