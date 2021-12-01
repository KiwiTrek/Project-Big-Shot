#include "RenderGlobals.h"
#include "Gameobject.h"

ComponentCamera::ComponentCamera(bool active) : Component(type, active) {
	type = ComponentTypes::CAMERA;

	frustrum.SetPos(float3(0.0f, 0.0f, -5.0f));
	frustrum.SetUp(float3(0.0f, 1.0f, 0.0f));
	frustrum.SetFront(float3(0.0f, 0.0f, 1.0f));

	//This function calculates the verticalFOV using the given horizontal FOV and aspect ratio. Also sets type to PerspectiveFrustum.
	frustrum.SetHorizontalFovAndAspectRatio(horizontalFOV * DEGTORAD, aspectRatio);

	frustrum.SetViewPlaneDistances(0.3f, 1000.0f);
}

ComponentCamera::~ComponentCamera() {}

void ComponentCamera::Update() {
	frustrum.SetPos(owner->GetComponent<ComponentTransform>()->GetPos());
	frustrum.SetUp(owner->GetComponent<ComponentTransform>()->GetGlobalTransform().WorldY());
	frustrum.SetFront(owner->GetComponent<ComponentTransform>()->GetGlobalTransform().WorldZ());

	DrawFrustrum();
}

void ComponentCamera::DrawFrustrum()
{
	float3 cornerPoints[8];
	frustrum.GetCornerPoints(cornerPoints);

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

//void ComponentCamera::DrawInspector()
//{
//	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
//	{
//		ImGui::Spacing();
//
//		bool fixedVerticalFOV = fixedFOV == FixedFOV::FIXED_VERTICAL_FOV;
//		bool fixedHorizontalFOV = fixedFOV == FixedFOV::FIXED_HORIZONTAL_FOV;
//
//		if (ImGui::RadioButton("Fixed VerticalFOV", fixedVerticalFOV))
//			fixedFOV = FixedFOV::FIXED_VERTICAL_FOV;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("Fixed HorizontalFOV", fixedHorizontalFOV))
//			fixedFOV = FixedFOV::FIXED_HORIZONTAL_FOV;
//
//		ImGui::Spacing();
//
//		//Fixed Vertical FOV Settings
//		if (fixedVerticalFOV)
//		{
//			float verticalFOV = _frustum.verticalFov * RADTODEG;
//			if (ImGui::SliderFloat("Vertical FOV", &verticalFOV, 20.0f, 60.0f))
//			{
//				_frustum.verticalFov = verticalFOV * DEGTORAD;
//				_frustum.horizontalFov = 2.0f * std::atan(std::tan(_frustum.verticalFov * 0.5f) * (_aspectRatio));
//			}
//
//			ImGui::Spacing();
//			ImGui::Text("Horizontal FOV: %.2f", _frustum.horizontalFov * RADTODEG);
//		}
//		//Fixed Horizontal FOV Settings
//		else
//		{
//			float horizontalFOV = _frustum.horizontalFov * RADTODEG;
//			if (ImGui::SliderFloat("Horizontal FOV", &horizontalFOV, 55.0f, 110.0f))
//			{
//				_frustum.horizontalFov = horizontalFOV * DEGTORAD;
//				_frustum.verticalFov = 2.0f * std::atan(std::tan(_frustum.horizontalFov * 0.5f) * (1 / _aspectRatio));
//			}
//			ImGui::Spacing();
//			ImGui::Text("Vertical FOV: %.2f", _frustum.verticalFov * RADTODEG);
//		}
//
//		ImGui::Spacing();
//		ImGui::Separator();
//		ImGui::Spacing();
//
//		ImGui::DragFloat("Near Plane", &_frustum.nearPlaneDistance, 0.05f, 100.0f);
//		ImGui::DragFloat("Far Plane", &_frustum.farPlaneDistance, 5.0f, 2000.0f);
//
//		ImGui::Spacing();
//	}
//}