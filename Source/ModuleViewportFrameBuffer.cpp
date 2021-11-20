#include "Application.h"
#include "ModuleViewportFrameBuffer.h"

#include "ModuleWindow.h"

#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "RenderGlobals.h"

ModuleViewportFrameBuffer::ModuleViewportFrameBuffer(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	show_viewport_window = true;
}

ModuleViewportFrameBuffer::~ModuleViewportFrameBuffer()
{}

bool  ModuleViewportFrameBuffer::Init()
{
	return true;
}

bool ModuleViewportFrameBuffer::Start()
{
	bool ret = false;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->window->GetWidth(), App->window->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	//Render Buffers
	glGenRenderbuffers(1, &renderBufferoutput);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferoutput);

	//Bind tex data with render buffers
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->GetWidth(), App->window->GetHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferoutput);

	//After binding tex data, we must unbind renderbuffer and framebuffer not usefull anymore
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	return true;
}

UpdateStatus ModuleViewportFrameBuffer::PreUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UpdateStatus::UPDATE_CONTINUE;
}


UpdateStatus ModuleViewportFrameBuffer::PostUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleViewportFrameBuffer::CleanUp()
{
	texture ? glDeleteTextures(1, &texture) : 0;
	frameBuffer ? glDeleteFramebuffers(1, &frameBuffer) : 0;
	renderBufferoutput ? glDeleteRenderbuffers(1, &renderBufferoutput) : 0;

	return true;
}