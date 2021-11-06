#ifndef __MESH_H__
#define __MESH_H__

#include "glmath.h"
#include "Color.h"
#include <vector>

class GameObject;

enum class MeshTypes
{
	NONE,
	Primitive_Grid,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Pyramid,
};

class Mesh : public Component
{
public:

	Mesh(bool active = true);
	virtual ~Mesh();

	virtual void Render() const;
	virtual void InnerRender() const;
	void DrawInspector();
	void DrawVertexNormals() const;
	void DrawFaceNormals() const;
	void GenerateBuffers();
	MeshTypes GetType() const;

public:
	Color color;
	bool axis, wire, drawVertexNormals, drawFaceNormals;

	uint indexBuf = -1;			// index buffer in VRAM
	int indexNum = -1;
	uint* indices = nullptr;

	uint vertexBuf = -1;		// vertex buffer in VRAM
	int vertexNum = -1;			// = normalsNum
	float* vertices = nullptr;

	uint normalsBuf = -1;		// normals buffer in VRAM
	float* normals = nullptr;

	float* texCoords = nullptr;	// = UVs
	float* colors = nullptr;

protected:
	MeshTypes mType;
};

// ============================================
class CubeP : public Mesh
{
public:
	CubeP(bool active = true);
};

// ============================================
class PlaneP : public Mesh
{
public:
	PlaneP(bool active = true);
};

// ============================================
class SphereP : public Mesh
{
public:
	SphereP(bool active = true);
	SphereP(float _radius, uint _meshRings, uint _quads, bool active = true);

	void InnerRender() const;

public:
	float radius;
	uint meshRings;
	uint quads;
};

// ============================================
class CylinderP : public Mesh
{
public:
	CylinderP(bool active = true);
	CylinderP(float _radius, float _height, uint _sides, bool active = true);

	void CalcGeometry();

public:
	float radius;
	float height;
	uint sides;
};

// ============================================
class PyramidP : public Mesh
{
public:
	PyramidP(bool active = true);
};

// ============================================
class Grid : public Mesh
{
public:
	Grid(bool active = true);
	Grid(float x, float y, float z, float d, bool active = true);

	void Render() const;

public:
	vec3 normal;
	float constant;
};

#endif // !__MESH_H__