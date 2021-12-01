#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Globals.h"
#include <string>
#include "MathGeoLib.h"

class GameObject;

class ComponentCamera : public Component
{
public:
	ComponentCamera(bool active = true);
	~ComponentCamera();

	void Update() override;
	//void DrawInspector() override;
	void DrawFrustrum();

public:
	Frustum frustrum;
	float horizontalFOV = 90.0f;
	float aspectRatio = 16 / 9;
};

#endif //!__COMPONENT_CAMERA_H__