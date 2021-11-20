#ifndef __MODULE_VIEWPORT_FRAME_BUFFER_H__
#define __MODULE_VIEWPORT_FRAME_BUFFER_H__

#include "Module.h"
#include "Globals.h"

#include <string>

class ModuleViewportFrameBuffer : public Module
{
public:
	ModuleViewportFrameBuffer(Application* app, bool start_enabled = true);
	~ModuleViewportFrameBuffer();

	bool Init() override;
	bool Start() override;
	UpdateStatus PreUpdate() override;
	UpdateStatus PostUpdate() override;
	bool CleanUp();

public:

	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint texture = 0;
	bool show_viewport_window = true;

};

#endif // !__MODULE_VIEWPORT_FRAME_BUFFER_H__