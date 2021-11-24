#ifndef __MESH_H__
#define __MESH_H__

#include "glmath.h"
#include "Color.h"
#include <vector>
#include "par_shapes.h"

class GameObject;

enum class Shape
{
	NONE,
	CUBE,
	SPHERE,
	CYLINDER,
	TORUS,
	PLANE,
	CONE
};

class Mesh : public Component
{
public:

	Mesh(bool active = true);
	Mesh(Shape shape, bool active = true);
	virtual ~Mesh();
	void CopyParMesh(par_shapes_mesh* parMesh);

	virtual void Render() const;
	virtual void InnerRender() const;
	void DrawInspector();
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;
	void GenerateBuffers();
	Shape GetType() const;

public:
	Color vertexColor;
	bool axis, wire, drawVertexNormals, drawFaceNormals;

	uint vertexBuf = 0, indexBuf = 0, textureBuf = 0, normalsBuf = 0;
	std::string texturePath;

	uint vertexNum = 0;
	std::vector<float3> vertices;

	uint normalNum = 0;
	std::vector<float3> normals;
	std::vector<float3> faceNormals;
	std::vector<float3> faceCenters;

	std::vector<float2> texCoords;

	uint indexNum = 0;
	std::vector<uint> indices;

	float* colors = nullptr;

protected:
	Shape mType;
};
#endif // !__MESH_H__