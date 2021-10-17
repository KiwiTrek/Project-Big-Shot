#ifndef __MESH_H__
#define __MESH_H__

#include "glmath.h"
#include "Color.h"
#include "Texture.h"
#include <vector>

enum MeshTypes
{
	Primitive_Grid,
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_Pyramid,
	Custom_Mesh,
	Mult_Mesh,
};

class Mesh
{
public:

	Mesh();
	virtual	~Mesh();

	void			GenerateBuffers();

	bool			SetTexture(Texture* texture);
	void			RemoveTexture();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			DrawVertexNormals() const;
	void			DrawFaceNormals() const;

	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);

	MeshTypes		GetType() const;

public:
	
	Color color;
	mat4x4 transform;
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

	uint textureBuf = -1;
	uint textureID = -1;
	Texture* texture = nullptr;
	float* texCoords = nullptr;
	float* colors = nullptr;

	bool drawVertexNormals;
	bool drawFaceNormals;

protected:
	void			SetDefaultTexture();
	MeshTypes type;
};

// ============================================
class CubeP : public Mesh
{
public :
	CubeP();
};

// ============================================
class PlaneP : public Mesh
{
public:
	PlaneP();
};

// ============================================
class SphereP : public Mesh
{
public:
	SphereP();
	SphereP(float _radius, uint _meshRings, uint _quads);
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
	CylinderP();
	CylinderP(float _radius, float _height, uint _sides);
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
	PyramidP();
};

// ============================================
class Grid : public Mesh
{
public:
	Grid();
	Grid(float x, float y, float z, float d);
	void Render() const;
public:
	vec3 normal;
	float constant;
};
#endif // !__MESH_H__