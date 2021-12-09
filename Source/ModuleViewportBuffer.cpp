#include "ModuleViewportBuffer.h"

#include "Application.h"
#include "RenderGlobals.h"
#include "Globals.h"

#include "ModuleWindow.h"
#include "ModuleGuiManager.h"

ModuleViewportBuffer::ModuleViewportBuffer(Application* app, bool startEnabled) : Module(app, startEnabled)
{}

ModuleViewportBuffer::~ModuleViewportBuffer()
{}

bool  ModuleViewportBuffer::Init()
{
	return true;
}

bool ModuleViewportBuffer::Start()
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
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);

	//Bind tex data with render buffers
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, App->window->GetWidth(), App->window->GetHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	//After binding tex data, we must unbind renderbuffer and framebuffer not usefull anymore
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

UpdateStatus ModuleViewportBuffer::PreUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return UpdateStatus::UPDATE_CONTINUE;
}


UpdateStatus ModuleViewportBuffer::PostUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleViewportBuffer::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool ModuleViewportBuffer::CleanUp()
{
	texture ? glDeleteTextures(1, &texture) : 0;
	frameBuffer ? glDeleteFramebuffers(1, &frameBuffer) : 0;
	renderBuffer ? glDeleteRenderbuffers(1, &renderBuffer) : 0;

	return true;
}