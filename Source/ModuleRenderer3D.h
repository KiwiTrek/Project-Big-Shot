#ifndef __MODULE_RENDERER_3D_H__
#define __MODULE_RENDERER_3D_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "Mesh.h"
#include <vector>

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool InitMeshes(std::vector<Mesh*> list);
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	void OnResize(int width, int height);
	bool GetVSync();
	void SetVSync(bool _vsync);
	bool IsDepthTest();
	void ToggleDepthTest();
	bool IsCullFace();
	void ToggleCullFace();
	bool IsLighting();
	void ToggleLighting();
	bool IsColorMaterial();
	void ToggleColorMaterial();
	bool IsTexture2D();
	void ToggleTexture2D();
	const char* GetVideoDriver();
	void ToggleWireframe();
	bool IsWireframe();
	void Render();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	bool vSync = false;
	bool wireframe = false;
	bool depthTest = true;
	bool cullFace = true;
	bool lighting = true;
	bool colorMaterial = true;
	bool texture2D = true;
};

#endif // !__MODULE_RENDERER_3D_H__