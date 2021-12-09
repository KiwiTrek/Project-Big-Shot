#ifndef __MODULE_VIEWPORT_BUFFER_H__
#define __MODULE_VIEWPORT_BUFFER_H__

#include "Module.h"
#include "Globals.h"

#include <string>

class ModuleViewportBuffer : public Module
{
public:
	ModuleViewportBuffer(Application* app, bool startEnabled = true);
	~ModuleViewportBuffer();

	bool Init() override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus PostUpdate() override;
	void OnResize(int width, int height);
	bool CleanUp();

public:

	uint frameBuffer = 0;
	uint renderBuffer = 0;
	uint texture = 0;

};

#endif // !__MODULE_VIEWPORT_BUFFER_H__