#include "RenderGlobals.h"
#include "Gameobject.h"

#include "Application.h"
#include "ModuleGameObjects.h"

ComponentCamera::ComponentCamera(bool active) : Component(type, active), fixedFOV(FixedFOV::HORIZONTAL), drawFrustum(true), drawBBox(false), culling(true), mainCamera(false)
{
	type = ComponentTypes::CAMERA;

	frustum.SetPos(float3(0.0f, 0.0f, -5.0f));
	frustum.SetUp(float3(0.0f, 1.0f, 0.0f));
	frustum.SetFront(float3(0.0f, 0.0f, 1.0f));

	//This function calculates the verticalFOV using the given horizontal FOV and aspect ratio. Also sets type to PerspectiveFrustum.
	frustum.SetHorizontalFovAndAspectRatio(horizontalFOV * DEG_TO_RAD, aspectRatio);
	frustum.SetViewPlaneDistances(5.0f, 100.0f);

	drawingBbox.SetFromCenterAndSize(vec(0.0f, 0.0f, 0.0f), vec(1.1f, 1.1f, 1.1f));
}

ComponentCamera::~ComponentCamera() {}

void ComponentCamera::Update()
{
	frustum.SetPos(owner->GetComponent<Transform>()->GetPos());
	frustum.SetUp(owner->GetComponent<Transform>()->GetGlobalTransform().WorldY());
	frustum.SetFront(owner->GetComponent<Transform>()->GetGlobalTransform().WorldZ());

	bbox.SetFromCenterAndSize(owner->GetComponent<Transform>()->GetPos(), vec(1.1f, 1.1f, 1.1f));
}

void ComponentCamera::Render() const
{
	float sx = 0.5f;
	float sy = 0.5f;
	float sz = 0.5f;

	//Draw Body
	float4x4 t = owner->GetComponent<Transform>()->GetGlobalTransform();
	glPushMatrix();
	glMultMatrixf((float*)&t.Transposed());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.5f);
	glBegin(GL_QUADS);

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(-sx, sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, sy, sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(-sx, -sy, sz);

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	glPopMatrix();

	if (drawFrustum) DrawFrustum();
}

void ComponentCamera::DrawFrustum() const
{
	float3 cornerPoints[8];
	frustum.GetCornerPoints(cornerPoints);

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
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

void ComponentCamera::DrawBBox() const
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

// tests if a BBox is within the frustum
bool ComponentCamera::ContainsBBox(const AABB& refBox) const
{
	float3 vertexCorner[8];

	//Get BBox
	refBox.GetCornerPoints(vertexCorner);

	for (int p = 0; p < 6; ++p)
	{
		int cornersOutside = 8;

		for (int i = 0; i < 8; ++i)
		{
			if (frustum.GetPlane(p).IsOnPositiveSide(vertexCorner[i])) --cornersOutside;
		}

		if (cornersOutside == 0) return false;
	}

	return true;
}

void ComponentCamera::DrawInspector(Application* App)
{
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::Button("Set as Main Camera", ImVec2(ImGui::CalcItemWidth(), 20)))
		{
			App->gameObjects->mainCamera->GetComponent<Camera>()->culling = false;
			App->gameObjects->mainCamera->GetComponent<Camera>()->mainCamera = false;
			App->gameObjects->mainCamera = owner;
			culling = false;
			mainCamera = true;
		}
		ImGui::Checkbox("Frustum Culling", &culling);

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		bool fixedVerticalFOV = fixedFOV == FixedFOV::VERTICAL;
		bool fixedHorizontalFOV = fixedFOV == FixedFOV::HORIZONTAL;

		ImGui::Text("Fixed");
		if (ImGui::RadioButton("VerticalFOV", fixedVerticalFOV)) fixedFOV = FixedFOV::VERTICAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("HorizontalFOV", fixedHorizontalFOV)) fixedFOV = FixedFOV::HORIZONTAL;

		ImGui::Spacing();

		if (fixedVerticalFOV)
		{
			//Fixed Vertical FOV Settings
			float verticalFOV = frustum.VerticalFov() * RAD_TO_DEG;
			if (ImGui::SliderFloat("Vertical FOV", &verticalFOV, 20.0f, 60.0f))
			{
				frustum.SetVerticalFovAndAspectRatio(verticalFOV * DEG_TO_RAD, aspectRatio);
			}

			ImGui::Spacing();
			ImGui::Text("Horizontal FOV: %.2f", frustum.HorizontalFov() * (double)RAD_TO_DEG);
		}
		else if (fixedHorizontalFOV)
		{
			//Fixed Horizontal FOV Settings
			float horizontalFOV = frustum.HorizontalFov() * RAD_TO_DEG;
			if (ImGui::SliderFloat("Horizontal FOV", &horizontalFOV, 55.0f, 110.0f))
			{
				frustum.SetHorizontalFovAndAspectRatio(horizontalFOV * DEG_TO_RAD, aspectRatio);
			}
			ImGui::Spacing();
			ImGui::Text("Vertical FOV: %.2f", frustum.VerticalFov() * (double)RAD_TO_DEG);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		float nearPlane = frustum.NearPlaneDistance();
		if (ImGui::SliderFloat("Near Plane", &nearPlane, 0.05f, 100.0f))
		{
			frustum.SetViewPlaneDistances(nearPlane, frustum.FarPlaneDistance());
		}
		float farPlane = frustum.FarPlaneDistance();
		if (ImGui::SliderFloat("Far Plane", &farPlane, 5.0f, 2000.0f))
		{
			frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), farPlane);
		}
	}
}

void ComponentCamera::OnLoad(const JSONReader& c, Application* App)
{
	if (c.HasMember("mainCamera"))
	{
		mainCamera = c["mainCamera"].GetBool();
		if (mainCamera) App->gameObjects->mainCamera = owner;
	}

	if (c.HasMember("fixedFOV"))
	{
		int fov = c["fixedFOV"].GetInt();
		switch (fov)
		{
		case 0:
			fixedFOV = FixedFOV::HORIZONTAL;
			break;
		case 1:
			fixedFOV = FixedFOV::VERTICAL;
			break;
		}
	}

	if (c.HasMember("culling")) culling = c["culling"].GetBool();
	if (c.HasMember("horizontalFOV")) horizontalFOV = c["horizontalFOV"].GetDouble();
	if (c.HasMember("aspectRatio")) aspectRatio = c["aspectRatio"].GetDouble();
}

void ComponentCamera::OnSave(JSONWriter& writer) const
{
	writer.String("Camera");
	writer.StartObject();
	writer.String("mainCamera"); writer.Bool(mainCamera);
	writer.String("fixedFOV"); writer.Int((int)fixedFOV);
	writer.String("culling"); writer.Bool(culling);
	writer.String("horizontalFOV"); writer.Double(horizontalFOV);
	writer.String("aspectRatio"); writer.Double(aspectRatio);
	writer.EndObject();
}