#ifndef __MODULE_CAMERA_3D_H__
#define __MODULE_CAMERA_3D_H__

#include "Module.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool startEnabled = true);
	~ModuleCamera3D();

	bool Start();
	UpdateStatus Update(float dt);
	bool CleanUp();

	void Look(const vec3& position, const vec3& reference, bool rotateAroundReference = false);
	void LookAt(const vec3& spot);
	void Move(const vec3& movement);
	float* GetViewMatrix();

private:
	void CalculateViewMatrix();

	void OnSave(JSONWriter& writer) const;

	void OnLoad(const JSONReader& reader);

public:
	vec3 x, y, z, position, reference;
	float currentDist;

private:
	mat4x4 viewMatrix, viewMatrixInverse;
};

#endif // !__MODULE_CAMERA_3D_H__