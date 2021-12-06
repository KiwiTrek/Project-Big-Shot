#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Globals.h"
#include <string>
#include "MathGeoLib.h"

class GameObject;
enum class FixedFOV
{
	HORIZONTAL,
	VERTICAL
};

class ComponentCamera : public Component
{
public:
	ComponentCamera(bool active = true);
	~ComponentCamera();

	void Update() override;
	//void DrawInspector() override;
	void Render() const;
	void DrawFrustum() const;
	void DrawBBox() const;
	void DrawInspector(Application* App);

	bool ContainsBBox(const AABB& refBox) const;

	void OnLoad(const JSONReader& mat, Application* App);
	void OnSave(JSONWriter& writer) const;

public:
	AABB bbox;
	AABB drawingBbox;
	Frustum frustum;
	FixedFOV fixedFOV;
	bool drawFrustum, drawBBox;
	bool culling;
	float horizontalFOV = 60.0f;
	float aspectRatio = 1.777f;
};

#endif //!__COMPONENT_CAMERA_H__