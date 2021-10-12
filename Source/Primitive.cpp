#include "Globals.h"
#include "RenderGlobals.h"
#include "Primitive.h"

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
CubeP::CubeP() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

CubeP::CubeP(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void CubeP::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

// SPHERE ============================================
SphereP::SphereP() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

SphereP::SphereP(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void SphereP::InnerRender() const
{
	int stacks = 16;
	int slices = 16;

	int i, j;
	for (j = 0; j < stacks; j++)
	{
		double lat1 = (M_PI / stacks) * j - M_PI / 2;
		double lat2 = (M_PI / stacks) * ((double)j + 1) - M_PI / 2;
		double sinLat1 = sin(lat1);
		double cosLat1 = cos(lat1);
		double sinLat2 = sin(lat2);
		double cosLat2 = cos(lat2);
		glBegin(GL_QUAD_STRIP);
		for (i = 0; i <= slices; i++)
		{
			double longitude = (2 * M_PI / slices) * i;
			double sinLong = sin(longitude);
			double cosLong = cos(longitude);
			double x1 = cosLong * cosLat1;
			double y1 = sinLong * cosLat1;
			double z1 = sinLat1;
			double x2 = cosLong * cosLat2;
			double y2 = sinLong * cosLat2;
			double z2 = sinLat2;
			glNormal3d(x2, y2, z2);
			glVertex3d(radius * x2, radius * y2, radius * z2);
			glNormal3d(x1, y1, z1);
			glVertex3d(radius * x1, radius * y1, radius * z1);
		}
		glEnd();
	}
}


// CYLINDER ============================================
CylinderP::CylinderP() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

CylinderP::CylinderP(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void CylinderP::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = (float)(i * M_PI / 180); // degrees to radians
		glVertex3f((GLfloat)(-height * 0.5f), (GLfloat)(radius * cos(a)), (GLfloat)(radius * sin(a)));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = (float)(i * M_PI / 180); // degrees to radians
		glVertex3f((GLfloat)(height * 0.5f), (GLfloat)(radius * cos(a)), (GLfloat)(radius * sin(a)));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = (float)(i * M_PI / 180); // degrees to radians

		glVertex3f((GLfloat)(height * 0.5f), (GLfloat)(radius * cos(a)), (GLfloat)(radius * sin(a)));
		glVertex3f((GLfloat)(-height * 0.5f), (GLfloat)(radius * cos(a)), (GLfloat)(radius * sin(a)));
	}
	glEnd();
}

// LINE ==================================================
Line::Line() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

Line::Line(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
PlaneP::PlaneP() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

PlaneP::PlaneP(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void PlaneP::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for(float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}

// PYRAMID ================================================
PyramidP::PyramidP() : Primitive(), base(1.0f, 1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Pyramid;
}

PyramidP::PyramidP(float baseX, float baseZ, float height) : Primitive(), base(baseX, baseZ), height(height)
{
	type = PrimitiveTypes::Primitive_Pyramid;
}

void PyramidP::InnerRender() const
{
	float bx = base.x * 0.5f;
	float bz = base.y * 0.5f;
	float sh = height * 0.5f;

	glBegin(GL_QUADS);

	//Base
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-bx, -sh, bz);
	glVertex3f(bx, -sh, bz);
	glVertex3f(bx, -sh, -bz);
	glVertex3f(-bx, -sh, -bz);
	
	glEnd();

	glBegin(GL_TRIANGLES);

	//Faces
	//vec3 edge1 = vec3(0 + bx, sh + sh, 0 - bz);
	//vec3 edge2 = vec3(bx + bx, -sh + sh, bz - bz);
	//vec3 normal = cross(edge2, edge1);
	//normalize(normal);
	//glNormal3f(normal.x, normal.y, normal.z);

	float ang = (float)atan(height / bz);
	glNormal3f(0.0f, ang, ang);

	glVertex3f(0, sh, 0);
	glVertex3f(-bx, -sh, bz);
	glVertex3f(bx, -sh, bz);

	//edge1 = vec3(0 - bx, sh + sh, 0 - bz);
	//edge2 = vec3(bx - bx, -sh + sh, bz + bz);
	//normal = cross(edge2, edge1);
	//normalize(normal);
	//glNormal3f(normal.x, normal.y, normal.z);

	ang = (float)atan(height / bx);
	glNormal3f(ang, ang, 0.0f);

	glVertex3f(0, sh, 0);
	glVertex3f(bx, -sh, bz);
	glVertex3f(bx, -sh, -bz);

	//edge1 = vec3(0 - bx, sh + sh, 0 + bz);
	//edge2 = vec3(bx + bx, -sh + sh, -bz + bz);
	//normal = cross(edge2, edge1);
	//normalize(normal);
	//glNormal3f(normal.x, normal.y, normal.z);

	ang = (float)atan(height / -bz);
	glNormal3f(0.0f, ang, ang);

	glVertex3f(0, sh, 0);
	glVertex3f(bx, -sh, -bz);
	glVertex3f(-bx, -sh, -bz);

	//edge1 = vec3(0 + bx, sh + sh, 0 + bz);
	//edge2 = vec3(-bx + bx, -sh + sh, -bz - bz);
	//normal = cross(edge2, edge1);
	//normalize(normal);
	//glNormal3f(normal.x, normal.y, normal.z);

	ang = (float)atan(height / -bx);
	glNormal3f(ang, ang, 0.0f);

	glVertex3f(0, sh, 0);
	glVertex3f(-bx, -sh, -bz);
	glVertex3f(-bx, -sh, bz);

	glEnd();
}