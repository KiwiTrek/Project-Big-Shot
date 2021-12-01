#ifndef __MESH_H__
#define __MESH_H__

#include "glmath.h"
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

	virtual void Render() const;
	virtual void InnerRender() const;
	void DrawInspector();
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;
	Shape GetType() const;
	void CreateBBox();
	void DrawBBox() const;

	//void OnLoad(const JSONReader& reader);
	//void OnSave(JSONWriter& writer) const;

public:
	Color vertexColor;
	bool axis, wire, wireOverride, drawVertexNormals, drawFaceNormals, drawBBox;
	AABB bbox;

	ResourceMesh* mesh;
};
#endif // !__MESH_H__