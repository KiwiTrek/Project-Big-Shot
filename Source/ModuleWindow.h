#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "Module.h"
#include "SDL.h"

#define DEFAULT_WIDTH 1028
#define DEFAULT_HEIGHT 720

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetTitle(const char* title);
	float GetBrightness();
	void SetBrightness(float b);
	int GetWidth();
	void SetWidth(int w);
	int GetHeight();
	void SetHeight(int h);
	void GetMaxWindow(int& w, int& h);
	uint GetRefreshRate();
	bool IsFullscreen();
	void SetFullscreen(bool f);
	bool IsResizable();
	void SetResizable(bool r);
	bool IsBorderless();
	void SetBorderless(bool b);
	bool IsFullscreenDesktop();
	void SetFullscreenDesktop(bool fd);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

private:
	int screenWidth = DEFAULT_WIDTH;
	int screenHeight = DEFAULT_HEIGHT;
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool fullscreenDesktop = false;
};

#endif // !__MODULE_WINDOW_H__