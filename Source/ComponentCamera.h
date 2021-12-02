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
	void DrawFrustum();

	bool ContainsBBox(const AABB& refBox) const;

public:
	Frustum frustum;
	float horizontalFOV = 60.0f;
	float aspectRatio = 1.777f;
};

#endif //!__COMPONENT_CAMERA_H__