#ifndef __MODULE_RENDERER_3D_H__
#define __MODULE_RENDERER_3D_H__

#include "Module.h"
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

	void DrawRay();

	const char* GetGlewVersion();
	const char* GetOpenGLVersion();
	const char* GetVideoDriver();

	void OnResize(int width, int height);
	bool GetVSync() { return vSync; };
	void SetVSync(bool vsync) { vSync = vsync; };
	bool IsDepthTest() { return depthTest; };
	void ToggleDepthTest();
	bool IsCullFace() { return cullFace; };
	void ToggleCullFace();
	bool IsLighting() { return lighting; };
	void ToggleLighting();
	bool IsColorMaterial() { return colorMaterial; };
	void ToggleColorMaterial();
	bool IsTexture2D() { return texture2D; };
	void ToggleTexture2D();
	bool IsWireframe() { return wireframe; };
	void ToggleWireframe() { wireframe = !wireframe; };

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	LineSegment ray;

private:
	bool depthTest = true;
	bool cullFace = true;
	bool lighting = false;
	bool colorMaterial = true;
	bool texture2D = true;
	bool vSync = false;

	bool wireframe = false;
	bool faceNormals = false;
	bool vecNormals = false;
};

#endif // !__MODULE_RENDERER_3D_H__