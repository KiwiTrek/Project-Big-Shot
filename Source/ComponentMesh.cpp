#include "RenderGlobals.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "Gameobject.h"

ComponentMesh::ComponentMesh(bool active) : Component(type, active), vertexColor(white), wire(false), wireOverride(false), drawFaceNormals(false), drawVertexNormals(false), normalLength(0.5f), drawBBox(false), render(true), centerPoint(float3::zero), radius(1.0f), mesh(nullptr)
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

void ComponentMesh::DrawInspector(Application* App)
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Mesh", &active);
		ImGui::SameLine();
		ImGui::Checkbox("Wireframe", &wire);
		ImGui::Separator();

		if (mesh != nullptr)
		{
			ImGui::Text("UID: "); ImGui::SameLine();
			ImGui::Button(std::to_string(mesh->GetUID()).c_str(), ImVec2(ImGui::CalcItemWidth(), 20));
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag mesh from 'Resources Panel' here to change it.");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESHES"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payloadUID = *(const int*)payload->Data;
					mesh = (ResourceMesh*)App->resources->RequestResource((UID)payloadUID);
					CreateBBox();
				}
				ImGui::EndDragDropTarget();
			}

			IMGUI_PRINT(IMGUI_YELLOW, "Path:", "%s", mesh->GetAssetFile());
			IMGUI_PRINT(IMGUI_YELLOW, "Vertices:", "%d", mesh->vertexNum);
			IMGUI_PRINT(IMGUI_YELLOW, "Indices:", "%d", mesh->indices);

			if (ImGui::Button("Remove mesh", ImVec2(ImGui::CalcItemWidth(), 20)))
			{
				mesh->referenceCount--;
				mesh = nullptr;
				CreateBBox();
			}

			ImGui::Separator();
		}
		else
		{
			ImGui::Text("UID: "); ImGui::SameLine();
			ImGui::Button("- None -", ImVec2(ImGui::CalcItemWidth(), 20));
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag mesh from 'Resources Panel' here to change it.");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESHES"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payloadUID = *(const int*)payload->Data;
					mesh = (ResourceMesh*)App->resources->RequestResource((UID)payloadUID);
					CreateBBox();
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::Checkbox("Vertex Normals", &drawVertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &drawFaceNormals);
		if (drawVertexNormals || drawFaceNormals) ImGui::SliderFloat("Length", &normalLength, 0.1f, 1.5f);
	}
}

void ComponentMesh::Render()
{
	if (mesh == nullptr) return;

	ComponentMaterial* mat = owner->GetComponent<Material>();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuf);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	//normals
	glBindBuffer(GL_NORMAL_ARRAY, mesh->normalsBuf);
	glNormalPointer(GL_FLOAT, 0, NULL);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuf);

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
			mat->material != nullptr ? glBindTexture(GL_TEXTURE_2D, mat->material->texId) : glBindTexture(GL_TEXTURE_2D, mat->checkersId);
		}
	}

	float4x4 t = owner->GetComponent<Transform>()->GetGlobalTransform();

	glPushMatrix();
	glMultMatrixf((float*)&t.Transposed());

	glColor3f(vertexColor.x, vertexColor.y, vertexColor.z);

	wire || wireOverride ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, mesh->indexNum, GL_UNSIGNED_INT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (drawFaceNormals) DrawFaceNormals();
	if (drawVertexNormals) DrawVertexNormals();

	glPopMatrix();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void ComponentMesh::DrawVertexNormals() const
{
	if (mesh->normalsBuf == -1 || mesh->normals.size() == 0) return;

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
		glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
		float vx = (mesh->vertices.at(i).x + mesh->vertices.at(i + 1).x + mesh->vertices.at(i + 2).x) / 3;
		float vy = (mesh->vertices.at(i).y + mesh->vertices.at(i + 1).y + mesh->vertices.at(i + 2).y) / 3;
		float vz = (mesh->vertices.at(i).z + mesh->vertices.at(i + 1).z + mesh->vertices.at(i + 2).z) / 3;

		float nx = (mesh->normals.at(i).x + mesh->normals.at(i + 1).x + mesh->normals.at(i + 2).x) / 3;
		float ny = (mesh->normals.at(i).y + mesh->normals.at(i + 1).y + mesh->normals.at(i + 2).y) / 3;
		float nz = (mesh->normals.at(i).z + mesh->normals.at(i + 1).z + mesh->normals.at(i + 2).z) / 3;

		glVertex3f(vx, vy, vz);
		glVertex3f(vx + (mesh->normals.at(i).x * normalLength),
			vy + (mesh->normals.at(i).y * normalLength),
			vz + (mesh->normals.at(i).z) * normalLength);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnd();
}

void ComponentMesh::CreateBBox()
{
	if (mesh != nullptr)
	{
		drawingBbox.SetNegativeInfinity();
		drawingBbox.Enclose(&mesh->vertices[0], mesh->vertices.size());

		bbox.SetNegativeInfinity();
		bbox.Enclose(&mesh->vertices[0], mesh->vertices.size());
	}
	else
	{
		drawingBbox.SetNegativeInfinity();
		drawingBbox.SetFromCenterAndSize(vec(0.0f, 0.0f, 0.0f), vec(1.0f, 1.0f, 1.0f));

		bbox.SetNegativeInfinity();
		bbox.SetFromCenterAndSize(vec(0.0f, 0.0f, 0.0f), vec(1.0f, 1.0f, 1.0f));
	}

	Sphere sphere;
	sphere.r = 0.f;
	sphere.pos = bbox.CenterPoint();
	sphere.Enclose(bbox);
	radius = sphere.r;
	centerPoint = sphere.pos;

	obb.SetFrom(bbox);
	obb.Transform(owner->GetComponent<Transform>()->GetGlobalTransform());

	bbox.SetNegativeInfinity();
	bbox.Enclose(obb);
}

void ComponentMesh::UpdateBBox()
{
	if (mesh != nullptr)
	{
		bbox.SetNegativeInfinity();
		bbox.Enclose(&mesh->vertices[0], mesh->vertices.size());
	}
	else
	{
		bbox.SetNegativeInfinity();
		bbox.SetFromCenterAndSize(owner->GetComponent<Transform>()->GetPos(), vec(1.0f, 1.0f, 1.0f));
	}

	obb.SetFrom(bbox);
	obb.Transform(owner->GetComponent<Transform>()->GetGlobalTransform());

	bbox.SetNegativeInfinity();
	bbox.Enclose(obb);
}

void ComponentMesh::DrawBBox() const
{
	float4x4 t = owner->GetComponent<Transform>()->GetGlobalTransform();

	glPushMatrix();
	glMultMatrixf((float*)&t.Transposed());

	float3 cornerPoints[8];
	drawingBbox.GetCornerPoints(cornerPoints);

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

	glPopMatrix();
}

float3 ComponentMesh::GetCenterPointInWorldCoords() const
{
	return owner->GetComponent<Transform>()->GetGlobalTransform().TransformPos(centerPoint);
}

void ComponentMesh::OnLoad(const JSONReader& m, Application* App)
{
	if (m.HasMember("Shape"))
	{
		int s = m["Shape"].GetInt();
		Shape shape;
		switch (s)
		{
		case 1:
			shape = Shape::CUBE;
			break;
		case 2:
			shape = Shape::SPHERE;
			break;
		case 3:
			shape = Shape::CYLINDER;
			break;
		case 4:
			shape = Shape::TORUS;
			break;
		case 5:
			shape = Shape::PLANE;
			break;
		case 6:
			shape = Shape::CONE;
			break;
		}
		ResourceMesh* rm = (ResourceMesh*)App->resources->RequestResource(shape);
		if (rm != nullptr) mesh = rm;
	}
	else if (m.HasMember("UID"))
	{
		ResourceMesh* rm = (ResourceMesh*)App->resources->RequestResource(m["UID"].GetInt());
		if (rm != nullptr) mesh = rm;
	}

	if (m.HasMember("vertexColor"))
	{
		const rapidjson::Value& jVertexColor = m["vertexColor"];
		float vc[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		uint i = 0;
		for (rapidjson::Value::ConstValueIterator it = jVertexColor.Begin(); it != jVertexColor.End(); ++it, ++i)
		{
			vc[i] = it->GetDouble();
		}
		vertexColor = Color(vc[0], vc[1], vc[2], vc[3]);
	}

	if (m.HasMember("wire")) wire = m["wire"].GetBool();
	if (m.HasMember("wireOverride")) wireOverride = m["wireOverride"].GetBool();
	if (m.HasMember("drawVertexNormals")) drawVertexNormals = m["drawVertexNormals"].GetBool();
	if (m.HasMember("drawFaceNormals")) drawFaceNormals = m["drawFaceNormals"].GetBool();

	if (m.HasMember("centerPoint"))
	{
		const rapidjson::Value& jCenterPoint = m["centerPoint"];
		float cp[3] = { 1.0f, 1.0f, 1.0f };
		uint i = 0;
		for (rapidjson::Value::ConstValueIterator it = jCenterPoint.Begin(); it != jCenterPoint.End(); ++it, ++i)
		{
			cp[i] = it->GetDouble();
		}
		centerPoint = float3(cp[0], cp[1], cp[2]);
	}

	if (m.HasMember("radius")) radius = m["radius"].GetDouble();
	if (m.HasMember("render")) render = m["render"].GetBool();

	drawBBox = false;
	if (mesh != nullptr) CreateBBox();
}

void ComponentMesh::OnSave(JSONWriter& writer) const
{
	writer.String("Mesh");
	writer.StartObject();
	writer.String("UID"); writer.Int(mesh->GetUID());
	if (mesh->mType != Shape::NONE)
	{
		writer.String("Shape"); writer.Int((int)mesh->mType);
	}
	writer.String("vertexColor");
	writer.StartArray();
	writer.Double(vertexColor.x);
	writer.Double(vertexColor.y);
	writer.Double(vertexColor.z);
	writer.Double(vertexColor.w);
	writer.EndArray();
	writer.String("wire"); writer.Bool(wire);
	writer.String("wireOverride"); writer.Bool(wireOverride);
	writer.String("drawVertexNormals"); writer.Bool(drawVertexNormals);
	writer.String("drawFaceNormals"); writer.Bool(drawFaceNormals);
	writer.String("centerPoint");
	writer.StartArray();
	writer.Double(centerPoint.x);
	writer.Double(centerPoint.y);
	writer.Double(centerPoint.y);
	writer.EndArray();
	writer.String("radius"); writer.Double(radius);
	writer.String("render"); writer.Bool(render);
	writer.EndObject();
}