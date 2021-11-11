#include "Globals.h"
#include "Application.h"
#include "RenderGlobals.h"

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "renderer";
}

ModuleRenderer3D::~ModuleRenderer3D()
{}

bool ModuleRenderer3D::Init()
{
	LOG_CONSOLE("Creating 3D Renderer context");
	bool ret = true;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG_CONSOLE("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		GLenum err = glewInit();

		if (err != GLEW_OK)
		{
			LOG_CONSOLE("Glew Init Error: %s\n", glewGetErrorString(err));
		}
		else
		{
			LOG_CONSOLE("Using Glew %s", glewGetString(GLEW_VERSION));
			LOG_CONSOLE("Vendor: %s", glGetString(GL_VENDOR));
			LOG_CONSOLE("Renderer: %s", glGetString(GL_RENDERER));
			LOG_CONSOLE("OpenGL version supported %s", glGetString(GL_VERSION));
			LOG_CONSOLE("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
		}

		//Use Vsync
		if (vSync && SDL_GL_SetSwapInterval(1) < 0)
		{
			LOG_CONSOLE("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		err = glGetError();
		if (err != GL_NO_ERROR)
		{
			LOG_CONSOLE("Error initializing OpenGL! %s\n", gluErrorString(err));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		err = glGetError();
		if (err != GL_NO_ERROR)
		{
			LOG_CONSOLE("Error initializing OpenGL! %s\n", gluErrorString(err));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		err = glGetError();
		if (err != GL_NO_ERROR)
		{
			LOG_CONSOLE("Error initializing OpenGL! %s\n", gluErrorString(err));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glDisable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		glShadeModel(GL_SMOOTH);
	}

	// Projection matrix for
	OnResize(App->window->GetWidth(), App->window->GetHeight());

	return ret;
}

UpdateStatus ModuleRenderer3D::PreUpdate()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
	{
		lights[i].Render();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRenderer3D::PostUpdate()
{
	SDL_GL_SwapWindow(App->window->window);
	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleRenderer3D::CleanUp()
{
	LOG_CONSOLE("Destroying 3D Renderer");

	normalMatrix.~mat3x3();
	modelMatrix.~mat4x4();
	viewMatrix.~mat4x4();
	projectionMatrix.~mat4x4();
	SDL_GL_DeleteContext(context);

	return true;
}

const char* ModuleRenderer3D::GetGlewVersion()
{
	return (const char*)glewGetString(GLEW_VERSION);
}

const char* ModuleRenderer3D::GetOpenGLVersion()
{
	return (const char*)glGetString(GL_VERSION);
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	projectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&projectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool ModuleRenderer3D::GetVSync()
{
	return vSync;
}

void ModuleRenderer3D::SetVSync(bool _vsync)
{
	vSync = _vsync;
}

const char* ModuleRenderer3D::GetVideoDriver()
{
	return SDL_GetCurrentVideoDriver();
}

void ModuleRenderer3D::ToggleWireframe()
{
	wireframe = !wireframe;
}

bool ModuleRenderer3D::IsWireframe()
{
	return wireframe;
}

void ModuleRenderer3D::ToggleDepthTest()
{
	depthTest = !depthTest;
	depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

bool ModuleRenderer3D::IsDepthTest()
{
	return depthTest;
}

void ModuleRenderer3D::ToggleCullFace()
{
	cullFace = !cullFace;
	cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

bool ModuleRenderer3D::IsCullFace()
{
	return cullFace;
}

void ModuleRenderer3D::ToggleLighting()
{
	lighting = !lighting;
	lighting ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
}

bool ModuleRenderer3D::IsLighting()
{
	return lighting;
}

void ModuleRenderer3D::ToggleAxis()
{
	axis = !axis;
}

bool ModuleRenderer3D::IsAxis()
{
	return axis;
}

void ModuleRenderer3D::ToggleColorMaterial()
{
	colorMaterial = !colorMaterial;
	colorMaterial ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);
}

bool ModuleRenderer3D::IsColorMaterial()
{
	return colorMaterial;
}

void ModuleRenderer3D::ToggleTexture2D()
{
	texture2D = !texture2D;
	texture2D ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
}

bool ModuleRenderer3D::IsTexture2D()
{
	return texture2D;
}