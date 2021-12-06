#ifndef __MESH_H__
#define __MESH_H__

#include "Color.h"
#include <vector>
#include "par_shapes.h"
#include "MathGeoLib.h"

class GameObject;
class ResourceMesh;
class ResourceMaterial;
enum class Shape;

class ComponentMesh : public Component
{
public:

	ComponentMesh(bool active = true);
	virtual ~ComponentMesh();

	void Update() override;
	void Render();
	void DrawInspector(Application* App);
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;
	Shape GetType() const;

	void CreateBBox();
	void UpdateBBox();
	void DrawBBox() const;

	float3 GetCenterPointInWorldCoords() const;

	void OnLoad(const JSONReader& m, Application* App);
	void OnSave(JSONWriter& writer) const;

public:
	Color vertexColor;
	bool wire, wireOverride, drawVertexNormals, drawFaceNormals, drawBBox;

	AABB bbox;
	AABB drawingBbox;
	OBB obb;
	float3 centerPoint;
	float radius;

	bool render;

	ResourceMesh* mesh;
};
#endif // !__MESH_H__