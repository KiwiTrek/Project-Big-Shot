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
	uint vertexNum = 0;
	std::vector<float3> vertices;

	uint normalNum = 0;
	std::vector<float3> normals;

	std::vector<float2> texCoords;

	uint indexNum = 0;
	std::vector<uint> indices;

	uint vertexBuf = 0;
	uint indexBuf = 0;
	uint textureBuf = 0;
	uint normalsBuf = 0;

	Shape mType;
};

#endif // !__RESOURCE_MESH_H__