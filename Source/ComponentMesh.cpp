#include "RenderGlobals.h"
#include "Gameobject.h"

Mesh::Mesh(bool active) : Component(type, active), vertexColor(white), wire(false), axis(false), mType(Shape::NONE), vertexBuf(-1), vertexNum(-1), indexBuf(-1), indexNum(-1),
normalsBuf(-1), colors(nullptr), drawFaceNormals(false), drawVertexNormals(false)
{
	type = ComponentTypes::MESH;
}

Mesh::Mesh(Shape shape, bool active) : Component(type, active), vertexColor(white), wire(false), axis(false), mType(shape), vertexBuf(-1), vertexNum(-1), indexBuf(-1), indexNum(-1),
normalsBuf(-1), colors(nullptr), drawFaceNormals(false), drawVertexNormals(false)
{
	switch (shape)
	{
	case Shape::CUBE:
		CopyParMesh(par_shapes_create_cube());
		break;
	case Shape::CYLINDER:
		CopyParMesh(par_shapes_create_cylinder(20, 20));
		break;
	case Shape::SPHERE:
		CopyParMesh(par_shapes_create_parametric_sphere(20, 20));
		break;
	case Shape::TORUS:
		CopyParMesh(par_shapes_create_torus(20, 20, 0.2));
		break;
	case Shape::PLANE:
		CopyParMesh(par_shapes_create_plane(20, 20));
		break;
	case Shape::CONE:
		CopyParMesh(par_shapes_create_cone(20, 20));
		break;
	}
}

Mesh::~Mesh()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexBuf);
	vertices.clear();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBuf);
	indices.clear();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &normalsBuf);
	normals.clear();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &textureBuf);
	texCoords.clear();

	delete colors;
	colors = nullptr;
}

void Mesh::CopyParMesh(par_shapes_mesh* parMesh)
{
	vertexNum = parMesh->npoints;
	indexNum = parMesh->ntriangles * 3;
	normalNum = parMesh->ntriangles;

	vertices.resize(vertexNum);
	normals.resize(normalNum);
	indices.resize(indexNum);
	texCoords.resize(vertexNum);

	par_shapes_compute_normals(parMesh);
	for (size_t i = 0; i < vertexNum; ++i)
	{
		memcpy(&vertices[i], &parMesh->points[i * 3], sizeof(float) * 3);
		memcpy(&normals[i], &parMesh->normals[i * 3], sizeof(float) * 3);
		if (parMesh->tcoords != nullptr)
		{
			memcpy(&texCoords[i], &parMesh->tcoords[i * 2], sizeof(float) * 2);
		}
		else if (mType == Shape::CUBE)
		{
			switch (i % 4)
			{
			case 0:
				texCoords.at(i).x = 0.0f;
				texCoords.at(i).y = 0.0f;
				break;
			case 1:
				texCoords.at(i).x = 1.0f;
				texCoords.at(i).y = 0.0f;
				break;
			case 2:
				texCoords.at(i).x = 1.0f;
				texCoords.at(i).y = 1.0f;
				break;
			case 3:
				texCoords.at(i).x = 0.0f;
				texCoords.at(i).y = 1.0f;
				break;
			}
		}
		else
		{
			texCoords.at(i).x = 0.0f;
			texCoords.at(i).y = 0.0f;
		}
	}
	for (size_t i = 0; i < indices.size(); ++i)
	{
		indices[i] = parMesh->triangles[i];
	}
	memcpy(&normals[0], parMesh->normals, vertexNum);

	par_shapes_free_mesh(parMesh);
}

Shape Mesh::GetType() const
{
	return mType;
}

void Mesh::GenerateBuffers()
{
	Material* mat = owner->GetComponent<Material>();

	//Vertex
	glGenBuffers(1, (GLuint*)&vertexBuf);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertexNum, &vertices[0], GL_STATIC_DRAW);

	//Normals
	glGenBuffers(1, &normalsBuf);
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuf);
	glBufferData(GL_NORMAL_ARRAY, sizeof(float) * vertexNum * 3, &normals[0], GL_STATIC_DRAW);

	//Indices
	glGenBuffers(1, &indexBuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, &indices[0], GL_STATIC_DRAW);

	if (mat != nullptr || !texCoords.empty())
	{
		//Textures
		glGenBuffers(1, &mat->textureBuf);
		glBindBuffer(GL_ARRAY_BUFFER, mat->textureBuf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
	}
}

void Mesh::DrawInspector()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Mesh", &active);
		ImGui::Separator();

		IMGUI_PRINT(IMGUI_YELLOW, "Vertices:", "%d", vertexNum);
		IMGUI_PRINT(IMGUI_YELLOW, "Indices:", "%d", indices);
		ImGui::Separator();

		ImGui::Checkbox("Vertex Normals", &drawVertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &drawFaceNormals);
	}
}

void Mesh::Render() const
{
	Material* mat = owner->GetComponent<Material>();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuf);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//normals
	glBindBuffer(GL_NORMAL_ARRAY, normalsBuf);
	glNormalPointer(GL_FLOAT, 0, NULL);

	if (mat != nullptr && mat->IsActive())
	{
		//textures
		glBindBuffer(GL_ARRAY_BUFFER, mat->textureBuf);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		glBindTexture(GL_TEXTURE_2D, mat->id);
	}

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuf);

	float4x4 t = owner->GetComponent<Transform>()->GetGlobalTransform();

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

	glColor3f(vertexColor.r, vertexColor.g, vertexColor.b);

	wire ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, NULL);


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

void Mesh::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	glPointSize(1.0f);
}

void Mesh::DrawVertexNormals() const
{
	if (normalsBuf == -1 || !normals.empty()) return;

	float normalLength = 0.5f;

	glBegin(GL_LINES);
	for (size_t i = 0; i < vertexNum * 3; ++i)
	{
		glColor4f(1.f, 0.0f, 0.0f, 1.f);
		glVertex3f(	vertices.at(i).x, vertices.at(i).y, vertices.at(i).z);
		glVertex3f(	vertices.at(i).x + (normals.at(i).x * normalLength),
					vertices.at(i).y + (normals.at(i).y * normalLength),
					vertices.at(i).z + (normals.at(i).z * normalLength));
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}

void Mesh::DrawFaceNormals() const
{
	if (normalsBuf == -1 || !normals.empty()) return;

	glBegin(GL_LINES);
	for (size_t i = 0; i < vertexNum * 3; ++i)
	{
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		float vx = (vertices.at(i).x + vertices.at(i + 1).x + vertices.at(i + 2).x) / 3;
		float vy = (vertices.at(i).y + vertices.at(i + 1).y + vertices.at(i + 2).y) / 3;
		float vz = (vertices.at(i).z + vertices.at(i + 1).z + vertices.at(i + 2).z) / 3;

		float nx = (normals.at(i).x + normals.at(i + 1).x + normals.at(i + 2).x) / 3;
		float ny = (normals.at(i).y + normals.at(i + 1).y + normals.at(i + 2).y) / 3;
		float nz = (normals.at(i).z + normals.at(i + 1).z + normals.at(i + 2).z) / 3;

		glVertex3f(vx, vy, vz);
		glVertex3f(vx + (normals.at(i).x * 0.5f),
			vy + (normals.at(i).y * 0.5f),
			vz + (normals.at(i).z) * 0.5f);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}