#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "Module.h"
#include "SDL.h"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool startEnabled = true);
	virtual ~ModuleWindow();

	bool Init();
	void OnLoad(const JSONReader& reader);
	void OnSave(JSONWriter& writer) const;
	bool CleanUp();

	void SetTitle(const char* title);
	uint GetRefreshRate();

	float GetBrightness();
	void SetBrightness(float b);

	int GetWidth();
	void SetWidth(int w);
	int GetHeight();
	void SetHeight(int h);
	void GetMaxWindow(int& w, int& h);

	bool IsFullscreen();
	void SetFullscreen(bool f);

	bool IsFullscreenDesktop();
	void SetFullscreenDesktop(bool fd);

	bool IsResizable();
	void SetResizable(bool r);

	bool IsBorderless();
	void SetBorderless(bool b);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screenSurface;

private:
	int screenWidth = DEFAULT_WIDTH;
	int screenHeight = DEFAULT_HEIGHT;
	bool fullscreen = false;
	bool resizable = true;
	bool borderless = false;
	bool fullscreenDesktop = false;
};

#endif // !__MODULE_WINDOW_H__