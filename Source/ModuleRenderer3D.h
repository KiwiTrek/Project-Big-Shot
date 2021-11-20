#ifndef __MODULE_RENDERER_3D_H__
#define __MODULE_RENDERER_3D_H__

#include "Module.h"
#include "glmath.h"
#include "Light.h"
#include "Mesh.h"
#include <vector>

#define MAX_LIGHTS 8

typedef unsigned char GLubyte;
typedef void* SDL_GLContext;

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool startEnabled = true);
	~ModuleRenderer3D();

	bool Init();
	UpdateStatus PreUpdate();
	UpdateStatus PostUpdate();
	void OnLoad(const JSONReader& reader);
	void OnSave(JSONWriter& writer) const;
	bool CleanUp();

	const char* GetGlewVersion();
	const char* GetOpenGLVersion();

	void OnResize(int width, int height);
	bool GetVSync();
	void SetVSync(bool _vsync);
	bool IsDepthTest();
	void ToggleDepthTest();
	bool IsCullFace();
	void ToggleCullFace();
	bool IsLighting();
	void ToggleLighting();
	bool IsAxis();
	void ToggleAxis();
	bool IsColorMaterial();
	void ToggleColorMaterial();
	bool IsTexture2D();
	void ToggleTexture2D();
	const char* GetVideoDriver();
	void ToggleWireframe();
	bool IsWireframe();

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 normalMatrix;
	mat4x4 modelMatrix, viewMatrix, projectionMatrix;

private:
	bool depthTest = true;
	bool cullFace = true;
	bool lighting = false;
	bool colorMaterial = true;
	bool texture2D = true;
	bool vSync = false;

	bool axis = false;
	bool wireframe = false;
	bool faceNormals = false;
	bool vecNormals = false;
};

#endif // !__MODULE_RENDERER_3D_H__