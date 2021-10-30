#ifndef __MESH_H__
#define __MESH_H__

#include "glmath.h"
#include "Color.h"
#include "ComponentMaterial.h"
#include <vector>

class GameObject;

enum class MeshTypes
{
	NONE,
	Primitive_Grid,
	//Primitive_Point,
	//Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Pyramid,
	//Custom_Mesh,
	//Mult_Mesh,
};

class Mesh : public Component
{
public:

	Mesh(bool active = true);
	virtual ~Mesh();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			DrawVertexNormals() const;
	void			DrawFaceNormals() const;
	void GenerateBuffers();

	MeshTypes		GetType() const;

public:
	
	Color color;
	bool axis,wire;

	uint indexBuf = -1; // index in VRAM
	int indexNum = -1;
	uint* indices = nullptr;

	uint vertexBuf = -1; // unique vertex in VRAM
	int vertexNum = -1; // = normalsNum
	float* vertices = nullptr;

	// normals for each face
	uint normalsBuf = -1;
	float* normals = nullptr;

	float* texCoords = nullptr;
	float* colors = nullptr;

	bool drawVertexNormals;
	bool drawFaceNormals;

protected:
	MeshTypes mType;
};

// ============================================
class CubeP : public Mesh
{
public :
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
	CylinderP( float _radius, float _height, uint _sides, bool active = true);
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