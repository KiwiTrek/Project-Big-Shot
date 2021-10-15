#ifndef __MESH_H__
#define __MESH_H__

#include "glmath.h"
#include "Color.h"

enum MeshTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Pyramid,
	Custom_Mesh,
};

struct MeshData
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* indices = nullptr;

	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertices = nullptr;

	// normals for each face
	//uint num_normals = 0;
	//vec3* normals = nullptr;
};

struct TexData
{
	int* data = nullptr;
	uint id = 0;
};

class Mesh
{
public:

	Mesh();

	virtual void	Render() const;
	virtual void	DrawTex() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	MeshTypes	GetType() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;
	TexData texture;

protected:
	MeshTypes type;
};

// ============================================
class CubeP : public Mesh
{
public :
	CubeP();
	CubeP(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
};

// ============================================
class SphereP : public Mesh
{
public:
	SphereP();
	SphereP(float radius);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class CylinderP : public Mesh
{
public:
	CylinderP();
	CylinderP(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class Line : public Mesh
{
public:
	Line();
	Line(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class PlaneP : public Mesh
{
public:
	PlaneP();
	PlaneP(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};

// ============================================
class PyramidP : public Mesh
{
public:
	PyramidP();
	PyramidP(float baseX, float baseZ, float height);
	void InnerRender() const;
public:
	vec2 base;
	float height;
};

class CustomMesh : public Mesh
{
public:
	CustomMesh();
	CustomMesh(MeshData* _data);
	void InnerRender() const;
public:
	MeshData* data;
};

#endif // !__MESH_H__