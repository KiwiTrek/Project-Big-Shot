#ifndef __RESOURCE_MESH_H__
#define __RESOURCE_MESH_H__

#include "Resource.h"
#include "par_shapes.h"

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

class ResourceMesh : public Resource
{
public:
	ResourceMesh(UID uid);
	ResourceMesh(UID uid, Shape shape);
	~ResourceMesh();

	void GenerateBuffers();

private:
	void CopyParMesh(par_shapes_mesh* parMesh);

public:
	uint vertexBuf = 0, indexBuf = 0, textureBuf = 0, normalsBuf = 0;

	uint vertexNum = 0;
	std::vector<float3> vertices;

	uint normalNum = 0;
	std::vector<float3> normals;

	std::vector<float2> texCoords;

	uint indexNum = 0;
	std::vector<uint> indices;

	Shape mType;
};

#endif // !__RESOURCE_MESH_H__