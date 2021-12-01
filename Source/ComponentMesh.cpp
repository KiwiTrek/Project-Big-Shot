#include "RenderGlobals.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "Gameobject.h"

ComponentMesh::ComponentMesh(bool active) : Component(type, active), vertexColor(white), wire(false), axis(false), drawFaceNormals(false), drawVertexNormals(false), drawBBox(false)
{
	type = ComponentTypes::MESH;
}

ComponentMesh::~ComponentMesh()
{
	mesh = nullptr;
}

Shape ComponentMesh::GetType() const
{
	return mesh->mType;
}

void ComponentMesh::DrawInspector()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Mesh", &active);
		ImGui::SameLine();
		ImGui::Checkbox("Wireframe", &wire);
		ImGui::Separator();

		IMGUI_PRINT(IMGUI_YELLOW, "Vertices:", "%d", mesh->vertexNum);
		IMGUI_PRINT(IMGUI_YELLOW, "Indices:", "%d", mesh->indices);
		ImGui::Separator();

		ImGui::Checkbox("Vertex Normals", &drawVertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &drawFaceNormals);
	}
}

void ComponentMesh::Render() const
{
	ComponentMaterial* mat = owner->GetComponent<ComponentMaterial>();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuf);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//normals
	glBindBuffer(GL_NORMAL_ARRAY, mesh->normalsBuf);
	glNormalPointer(GL_FLOAT, 0, NULL);

	if (mat != nullptr && mat->IsActive())
	{
		//textures
		glBindBuffer(GL_ARRAY_BUFFER, mesh->textureBuf);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		if (mat->usingCheckers)
		{
			glBindTexture(GL_TEXTURE_2D, mat->checkersId);
		}
		else
		{
			if (mat->material != nullptr) glBindTexture(GL_TEXTURE_2D, mat->material->texId);
			else glBindTexture(GL_TEXTURE_2D, mat->checkersId);
		}
	}

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuf);

	float4x4 t = owner->GetComponent<ComponentTransform>()->GetGlobalTransform();

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

	wire || wireOverride ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, mesh->indexNum, GL_UNSIGNED_INT, NULL);

	if (drawFaceNormals) DrawFaceNormals();
	if (drawVertexNormals) DrawVertexNormals();

	if (drawBBox) DrawBBox();

	glPopMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ComponentMesh::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();

	glPointSize(1.0f);
}

void ComponentMesh::DrawVertexNormals() const
{
	if (mesh->normalsBuf == -1 || mesh->normals.size() == 0) return;

	float normalLength = 0.5f;

	glBegin(GL_LINES);
	for (size_t i = 0; i < mesh->vertices.size(); ++i)
	{
		glColor4f(1.f, 0.0f, 0.0f, 1.f);
		glVertex3f(mesh->vertices.at(i).x, mesh->vertices.at(i).y, mesh->vertices.at(i).z);
		glVertex3f(mesh->vertices.at(i).x + (mesh->normals.at(i).x * normalLength),
				mesh->vertices.at(i).y + (mesh->normals.at(i).y * normalLength),
				mesh->vertices.at(i).z + (mesh->normals.at(i).z * normalLength));
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}

void ComponentMesh::DrawFaceNormals() const
{
	if (mesh->normalsBuf == -1 || mesh->normals.size() == 0) return;

	glBegin(GL_LINES);
	for (size_t i = 0; i < mesh->vertices.size() - 3; ++i)
	{
		glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
		float vx = (mesh->vertices.at(i).x + mesh->vertices.at(i + 1).x + mesh->vertices.at(i + 2).x) / 3;
		float vy = (mesh->vertices.at(i).y + mesh->vertices.at(i + 1).y + mesh->vertices.at(i + 2).y) / 3;
		float vz = (mesh->vertices.at(i).z + mesh->vertices.at(i + 1).z + mesh->vertices.at(i + 2).z) / 3;

		float nx = (mesh->normals.at(i).x + mesh->normals.at(i + 1).x + mesh->normals.at(i + 2).x) / 3;
		float ny = (mesh->normals.at(i).y + mesh->normals.at(i + 1).y + mesh->normals.at(i + 2).y) / 3;
		float nz = (mesh->normals.at(i).z + mesh->normals.at(i + 1).z + mesh->normals.at(i + 2).z) / 3;

		glVertex3f(vx, vy, vz);
		glVertex3f(vx + (mesh->normals.at(i).x * 0.5f),
			vy + (mesh->normals.at(i).y * 0.5f),
			vz + (mesh->normals.at(i).z) * 0.5f);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}

void ComponentMesh::CreateBBox()
{
	bbox.SetNegativeInfinity();

	float3* vertices = new float3[mesh->vertexNum];

	for (size_t i = 0; i < mesh->vertexNum; i++)
	{
		vertices[i].x = mesh->vertices[i].x;
		vertices[i].y = mesh->vertices[i].y;
		vertices[i].z = mesh->vertices[i].z;
	}

	bbox.Enclose(vertices, mesh->vertexNum);

	delete[] vertices;
}

void ComponentMesh::DrawBBox() const
{
	float3 cornerPoints[8];
	bbox.GetCornerPoints(cornerPoints);

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glLineWidth(3.5f);
	glBegin(GL_LINES);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}

//void ComponentMesh::OnLoad(const JSONReader& reader)
//{
//
//}
//
//void ComponentMesh::OnSave(JSONWriter& writer) const
//{
//	//writer.StartObject();
//	//writer.StartArray();
//	//writer.Int(vertexColor.r);
//	//writer.Int(vertexColor.g);
//	//writer.Int(vertexColor.b);
//	//writer.Int(vertexColor.a);
//	//writer.EndArray();
//	//writer.Bool(axis);
//	//writer.Bool(wire);
//	//writer.Bool(drawVertexNormals);
//	//writer.Bool(drawFaceNormals);
//	//writer.Int(vertexBuf);
//	//writer.Int(indexBuf);
//	//writer.Int(textureBuf);
//	//writer.Int(normalsBuf);
//	//writer.Int(vertexNum);
//	//
//	//writer.EndObject();
//}