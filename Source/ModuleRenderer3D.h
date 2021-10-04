#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "Primitive.h"
#include <vector>

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	void OnResize(int width, int height);
	bool GetVSync();
	void SetVSync(bool vsync);
	const char* GetVideoDriver();
	void ToggleWireframe();
	bool IsWireframe();
	void AddPrimitive(Primitive* p);
	void Render();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	std::vector<Primitive*> listPrimitives;
private:
	bool vSync = false;
	bool wireframe = false;
};