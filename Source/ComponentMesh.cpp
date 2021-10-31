#include "Globals.h"
#include "RenderGlobals.h"
#include "Gameobject.h"

#include "Component.h"
#include "ComponentMesh.h"

// ------------------------------------------------------------
Mesh::Mesh(bool active) : Component(type,active), color(White), wire(false), axis(false), mType(MeshTypes::NONE), vertexBuf(-1), vertexNum(-1), vertices(nullptr), indexBuf(-1), indexNum(-1), indices(nullptr),
normalsBuf(-1), texCoords(nullptr), normals(nullptr), colors(nullptr), drawFaceNormals(false), drawVertexNormals(false)
{
	type = ComponentTypes::MESH;
}

Mesh::~Mesh()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexBuf);
	delete vertices;
	vertices = nullptr;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBuf);
	delete indices;
	indices = nullptr;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &normalsBuf);
	delete normals;
	normals = nullptr;

	delete colors;
	colors = nullptr;

	delete texCoords;
	texCoords = nullptr;
}

// ------------------------------------------------------------
MeshTypes Mesh::GetType() const
{
	return mType;
}

// ------------------------------------------------------------
void Mesh::GenerateBuffers()
{
	Material* mat = nullptr;
	std::vector<Component*>::iterator c = owner->components.begin();
	while (c != owner->components.end())
	{
		if ((*c)->type == ComponentTypes::MATERIAL)
		{
			mat = (Material*)(*c);
		}
		c++;
	}

	//Vertex
	glGenBuffers(1, (GLuint*)&vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNum * 3, vertices, GL_STATIC_DRAW);

	//Normals
	glGenBuffers(1, &normalsBuf);
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuf);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * vertexNum * 3, normals, GL_STATIC_DRAW);

	//Indices
	glGenBuffers(1, &indexBuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, indices, GL_STATIC_DRAW);

	if (mat != nullptr)
	{
		//Textures
		glGenBuffers(1, &mat->textureBuf);
		glBindBuffer(GL_ARRAY_BUFFER, mat->textureBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNum * 2, texCoords, GL_STATIC_DRAW);
	}
}

// ------------------------------------------------------------
void Mesh::DrawInspector()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enabled", &active);
		ImGui::Spacing();

		IMGUI_PRINT(IMGUI_YELLOW, "Vertices:", "%d", vertexNum);
		IMGUI_PRINT(IMGUI_YELLOW, "Indices:", "%d", indices);
		ImGui::Spacing();

		ImGui::Checkbox("Vertex Normals", &drawVertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &drawFaceNormals);
	}
}

// ------------------------------------------------------------
void Mesh::Render() const
{
	Material* mat = nullptr;
	std::vector<Component*>::iterator c = owner->components.begin();
	while (c != owner->components.end())
	{
		if ((*c)->type == ComponentTypes::MATERIAL)
		{
			mat = (Material*)(*c);
		}
		c++;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//normals
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuf);
	glNormalPointer(GL_FLOAT, 0, NULL);

	if (mat != nullptr)
	{
		//textures
		glBindBuffer(GL_ARRAY_BUFFER, mat->textureBuf);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		glBindTexture(GL_TEXTURE_2D, mat->id);
	}

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);

	float4x4 t = owner->GetTransform()->GetGlobalTransform();

	glPushMatrix();
	glMultMatrixf((float*)&t.Transposed());

	if (axis == true)
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

	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (mType == MeshTypes::Primitive_Sphere)
	{
		InnerRender();
	}
	else
	{
		glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, NULL);
	}

	if (drawFaceNormals) DrawFaceNormals();
	if (drawVertexNormals) DrawVertexNormals();

	glPopMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

// ------------------------------------------------------------
void Mesh::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Mesh::DrawVertexNormals() const
{
	if (normalsBuf == -1 || normals == nullptr)
		return;

	float normal_lenght = 0.5f;

	//vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0, c = 0; i < vertexNum * 3; i += 3, c += 4)
	{
		glColor4f(1.f, 0.0f, 0.0f, 1.f);
		//glColor4f(colors[c], colors[c + 1], colors[c + 2], colors[c + 3]);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);

		glVertex3f(vertices[i] + (normals[i] * normal_lenght),
			vertices[i + 1] + (normals[i + 1] * normal_lenght),
			vertices[i + 2] + (normals[i + 2]) * normal_lenght);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}

// ------------------------------------------------------------
void Mesh::DrawFaceNormals() const
{
	if (normalsBuf == -1 || normals == nullptr)
		return;

	//vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0; i < vertexNum * 3; i += 3)
	{
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		float vx = (vertices[i] + vertices[i + 3] + vertices[i + 6]) / 3;
		float vy = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;
		float vz = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;

		float nx = (normals[i] + normals[i + 3] + normals[i + 6]) / 3;
		float ny = (normals[i + 1] + normals[i + 4] + normals[i + 7]) / 3;
		float nz = (normals[i + 2] + normals[i + 5] + normals[i + 8]) / 3;

		glVertex3f(vx, vy, vz);

		glVertex3f(vx + (normals[i] * 0.5f),
			vy + (normals[i + 1] * 0.5f),
			vz + (normals[i + 2]) * 0.5f);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glEnd();
}

// CUBE ===============================================
CubeP::CubeP(bool active) : Mesh(active)
{
	mType = MeshTypes::Primitive_Cube;

	vertices = new float[72]
	{
		//Low vertices
		-0.5f, -0.5f, -0.5f, //Bottom Left			1
		0.5f, -0.5f, -0.5f, //Bottom Right			2
		0.5f, -0.5f, 0.5f, //Top Right				3
		-0.5f, -0.5f, 0.5f, //Top Left				4

		//High vertices
		-0.5f, 0.5f, -0.5f, //Bottom Left			5
		0.5f, 0.5f, -0.5f, //Bottom Right			6
		0.5f, 0.5f, 0.5f, //Top Right				7
		-0.5f, 0.5f, 0.5f, //Top Left				8

		// Extra points for texcoords
		-0.5f, -0.5f, 0.5f, //Top Left				5
		-0.5f, -0.5f, -0.5f, //Bottom Left			1
		-0.5f, -0.5f, 0.5f, //Top Left				4
		0.5f, 0.5f, 0.5f, //Top Right				8

		-0.5f, -0.5f, 0.5f, //Top Left				5
		-0.5f, 0.5f, -0.5f, //Bottom Left			6
		0.5f, -0.5f, -0.5f, //Bottom Right			2
		-0.5f, -0.5f, -0.5f, //Bottom Left			1

		0.5f, -0.5f, -0.5f, //Bottom Right			2
		0.5f, 0.5f, -0.5f, //Bottom Right			6
		0.5f, 0.5f, -0.5f, //Bottom Right			7
		0.5f, -0.5f, 0.5f, //Top Right				3

		-0.5f, -0.5f, 0.5f, //Top Left				4
		0.5f, -0.5f, 0.5f, //Top Right				3
		0.5f, 0.5f, 0.5f, //Top Right				7
		0.5f, 0.5f, 0.5f, //Top Right				8
	};

	indices = new uint[36]
	{
		// Faces
		0,1,2, 2,3,0, //Bottom
		3,2,6, 6,7,3, //Front
		2,1,5, 5,6,2, //Right
		7,4,0, 0,3,7, //Left
		1,0,4, 4,5,1, //Back
		5,4,7, 7,6,5 //Top
	};

	texCoords = new float[48]
	{
		0.0f, 0.5f, //Top Left			5
		0.25f, 0.5f, //Bottom Left		1
		0.25f, 0.25f, //Top Left		4
		0.0f, 0.25f, //Top Right		8

		0.25f, 0.5f, //Bottom Left		1
		0.5f, 0.5f, //Bottom Right		2
		0.5f, 0.25f, //Top Right		3
		0.25f, 0.25f, //Top Left		4

		0.5f, 0.5f, //Bottom Right		2
		0.75f, 0.5f, //Bottom Right		6
		0.75f, 0.25f, //Bottom Right	7
		0.5f, 0.25f, //Top Right		3

		1.0f, 0.5f, //Bottom Left		5
		0.75f, 0.5f, //Bottom Right		6
		0.75f, 0.25f, //Top Right		7
		1.0f, 0.25f, //Top Left			8

		0.25f, 0.75f, //Top Left		5
		0.5f, 0.75f, //Bottom Left		6
		0.5f, 0.5f, //Bottom Right		2
		0.25f, 0.5f, //Bottom Left		1

		0.25f, 0.25f, //Top Left		4
		0.5f, 0.25f, //Top Right		3
		0.5f, 0.0f, //Top Right			7
		0.25f, 0.0f, //Top Right		8
	};

	vertexNum = 24;
	indexNum = 36;
}

// PLANE ==============================================
PlaneP::PlaneP(bool active) : Mesh(active)
{
	mType = MeshTypes::Primitive_Plane;

	vertices = new float[12]
	{
		-0.5f, 0.0f, -0.5f, //Bottom Left
		0.5f, 0.0f, -0.5f, //Bottom Right
		0.5f, 0.0f, 0.5f, //Top Right
		-0.5f, 0.0f, 0.5f, //Top Left
	};

	indices = new uint[6]{ 0,3,2, 2,1,0 };

	texCoords = new float[8]
	{
		//Low vertices
		0.0f, 0.0f, //Bottom Left
		1.0f, 0.0f, //Bottom Right
		1.0f, 1.0f, //Top Right
		0.0f, 1.0f //Top Left
	};

	vertexNum = 4;
	indexNum = 6;
}

// SPHERE ============================================
SphereP::SphereP(bool active) : Mesh(active), radius(1), meshRings(12), quads(24)
{
	mType = MeshTypes::Primitive_Sphere;
}

SphereP::SphereP(float _radius, uint _meshRings, uint _quads, bool active) : Mesh(active), radius(_radius), meshRings(_meshRings), quads(_quads)
{
	mType = MeshTypes::Primitive_Sphere;
}

void SphereP::InnerRender() const
{
	int i, j;
	for (j = 0; j < meshRings; j++)
	{
		double latitude1 = (M_PI / meshRings) * j - M_PI / 2;
		double latitude2 = (M_PI / meshRings) * (j + 1) - M_PI / 2;
		double sinLat1 = sin(latitude1);
		double cosLat1 = cos(latitude1);
		double sinLat2 = sin(latitude2);
		double cosLat2 = cos(latitude2);
		glBegin(GL_QUAD_STRIP);
		for (i = 0; i <= quads; i++)
		{
			double longitude = (2 * M_PI / quads) * i;
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

// CYLINDER ==========================================
CylinderP::CylinderP(bool active) : Mesh(active), radius(1), height(1), sides(16)
{
	mType = MeshTypes::Primitive_Cylinder;
	CalcGeometry();
}

CylinderP::CylinderP(float _radius, float _height, uint _sides, bool active) : Mesh(active), radius(_radius), height(_height), sides(_sides)
{
	mType = MeshTypes::Primitive_Cylinder;
	CalcGeometry();
}

void CylinderP::CalcGeometry()
{
	float currentAngle = 0;
	float increase = 2 * M_PI / sides;

	//Vertices ------------------------------------------------

	std::vector<float> verticesTMP;

	//Top center
	verticesTMP.push_back(0);
	verticesTMP.push_back(height * 0.5f);
	verticesTMP.push_back(0);

	//Top face
	for (int i = 0; i < sides; i++)
	{
		verticesTMP.push_back(radius * cos(currentAngle));//x
		verticesTMP.push_back(height * 0.5f);		       //y
		verticesTMP.push_back(radius * sin(currentAngle));//z

		//anticlockwise
		currentAngle -= increase;
	}

	currentAngle = 0;

	//Bottom Center
	verticesTMP.push_back(0);
	verticesTMP.push_back(-height * 0.5f);
	verticesTMP.push_back(0);

	//Bottom face
	for (int i = 0; i < sides; i++)
	{
		verticesTMP.push_back(radius * cos(currentAngle)); //x
		verticesTMP.push_back(-height * 0.5f);			    //y
		verticesTMP.push_back(radius * sin(currentAngle)); //z

		//clockwise
		currentAngle -= increase;
	}

	// Indices ----------------------------------------------

	std::vector<uint> indicesTMP;

	//Top Face
	for (int i = 1; i < sides; i++)
	{
		indicesTMP.push_back(0);
		indicesTMP.push_back(i);
		indicesTMP.push_back(i + 1);
	}

	indicesTMP.push_back(0);
	indicesTMP.push_back(sides);
	indicesTMP.push_back(1);

	//Sides
	for (int i = 1; i < sides; i++)
	{
		//Left triangle
		indicesTMP.push_back(i);
		indicesTMP.push_back(sides + i + 1);
		indicesTMP.push_back(sides + i + 2);

		//Right triangle
		indicesTMP.push_back(i + sides + 2);
		indicesTMP.push_back(i + 1);
		indicesTMP.push_back(i);
	}

	indicesTMP.push_back(sides);
	indicesTMP.push_back(2 * sides + 1);
	indicesTMP.push_back(sides + 2);

	indicesTMP.push_back(sides + 2);
	indicesTMP.push_back(1);
	indicesTMP.push_back(sides);

	//Bottom Face
	int k = sides + 1;
	for (int i = 1; i < sides; i++)
	{
		indicesTMP.push_back(k);
		indicesTMP.push_back(k + i + 1);
		indicesTMP.push_back(k + i);
	}

	indicesTMP.push_back(k);
	indicesTMP.push_back(sides + 2);
	indicesTMP.push_back(2 * sides + 1);

	//TODO: Texcoords

	vertexNum = verticesTMP.size();
	vertices = new float[vertexNum]();

	for (size_t i = 0; i < vertexNum; i++)
	{
		vertices[i] = verticesTMP[i];
	}

	indexNum = indicesTMP.size();
	indices = new uint[indexNum]();

	for (size_t i = 0; i < indexNum; i++)
	{
		indices[i] = indicesTMP[i];
	}

	verticesTMP.clear();
	indicesTMP.clear();
}

// PYRAMID ===========================================
PyramidP::PyramidP(bool active) : Mesh(active)
{
	mType = MeshTypes::Primitive_Pyramid;

	vertices = new float[15]
	{
		//Top Vertex
		0.0f, 0.85f, 0.0f,

		//Bottom 
		-0.5f ,0.0f, -0.5f,
		0.5f ,0.0f, -0.5f,
		0.5f ,0.0f, 0.5f,
		-0.5f ,0.0f, 0.5f
	};

	indices = new uint[18]
	{
		0, 4, 3, // Front
		0, 3, 2, // Left
		0, 2, 1, // Right
		0, 1, 4,  // Back

		1, 3, 4,  1, 2, 3 //Bottom
	};

	texCoords = new float[10]
	{
		//High vertex
		0.5f, 1.0f,

		//Low vertices
		0.0f, 0.0f, //Bottom Left
		1.0f, 0.0f, //Bottom Right
		0.0f, 0.0f, //Top Right
		1.0f, 0.0f,
	};

	vertexNum = 5;
	indexNum = 18;
}

// GRID ==============================================
Grid::Grid(bool active) : Mesh(active), normal(0, 1, 0), constant(1)
{
	mType = MeshTypes::Primitive_Grid;
}

Grid::Grid(float x, float y, float z, float d, bool active) : Mesh(active), normal(x, y, z), constant(d)
{
	mType = MeshTypes::Primitive_Grid;
}

void Grid::Render() const
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