#ifndef __MODULE_CAMERA_3D_H__
#define __MODULE_CAMERA_3D_H__

#include "Module.h"
#include "Globals.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool startEnabled = true);
	~ModuleCamera3D();

	bool Start() override;
	UpdateStatus Update(float dt)override;
	bool CleanUp() override;

	void LookAt(const float3& point);
	void CalculateViewMatrix();
	void RecalculateProjection();
	GameObject* MousePicking();
	void OnSave(JSONWriter& writer) const override;
	void OnLoad(const JSONReader& reader) override;

	float3 right, up, front, position, reference;
	Frustum cameraFrustum;
	float4x4 viewMatrix;
	float aspectRatio = 1.f;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 1000.f;
	float cameraSensitivity = .1f;
	float cameraSpeed = 60.f;
	bool projectionIsDirty = false;

private:
	float lastDeltaX = 0.f, lastDeltaY = 0.f;

};

#endif // !__MODULE_CAMERA_3D_H__