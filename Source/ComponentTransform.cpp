#include "Component.h"
#include "ComponentTransform.h"
#include "Gameobject.h"

Transform::Transform(bool active) : Component(type, active)
{
	type = ComponentTypes::TRANSFORM;

	pos = float3::zero;
	rot = Quat::identity;
	eulerRot = float3::zero;
	scale = float3::one;

	lTransform = float4x4::FromTRS(pos, rot, scale);
	gTransform = lTransform;
	gParentTransform = float4x4::identity;
}

Transform::Transform(float3 p, Quat r, float3 s, bool active) : Component(type, active)
{
	type = ComponentTypes::TRANSFORM;

	pos = p;
	rot = r;
	eulerRot = r.ToEulerXYZ() * RADTODEG;
	scale = s;

	lTransform = float4x4::FromTRS(pos, rot, scale);
	gTransform = lTransform;
	gParentTransform = float4x4::identity;
}

Transform::~Transform()
{
	pos.zero;
	rot.identity;
	eulerRot.zero;
	scale.one;

	lTransform.zero;
	gTransform.zero;
	gParentTransform.zero;
}

void Transform::Reset()
{
	pos = float3::zero;
	rot = Quat::identity;
	eulerRot = float3::zero;
	scale = float3::one;

	UpdateGlobalTransform();
	owner->UpdateChildrenTransforms();
}

void Transform::UpdateGlobalTransform()
{
	UpdateLocalTransform();
	gTransform = gParentTransform * lTransform;
}

void Transform::UpdateGlobalTransform(float4x4 pGlobalTransform)
{
	UpdateLocalTransform();
	gParentTransform = pGlobalTransform;
	gTransform = gParentTransform * lTransform;
}

void Transform::UpdateLocalTransform()
{
	lTransform = float4x4::FromTRS(pos, rot, scale);
	eulerRot = rot.ToEulerXYZ() * RADTODEG;
}

void Transform::DrawInspector()
{
	if (owner->parent != nullptr)
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			float position4f[4] = { pos.x, pos.y, pos.z, 1.0f };
			if (ImGui::DragFloat3("Position", position4f, 0.1f, -10000.0f, 10000.0f))
			{
				SetPos(position4f[0], position4f[1], position4f[2]);
				UpdateGlobalTransform();
				owner->UpdateChildrenTransforms();
			}

			float rotation4f[4] = { eulerRot.x, eulerRot.y, eulerRot.z, 1.0f };
			if (ImGui::DragFloat3("Rotation", rotation4f, 0.1f, -360.0f, 360.0f))
			{
				SetRot(rotation4f[0], rotation4f[1], rotation4f[2]);
				UpdateGlobalTransform();
				owner->UpdateChildrenTransforms();
			}

			float scale4f[4] = { scale.x, scale.y, scale.z, 1.0f };
			if (ImGui::DragFloat3("Scale", scale4f, 0.01f, -10000.0f, 10000.0f))
			{
				SetScale(scale4f[0], scale4f[1], scale4f[2]);
				UpdateGlobalTransform();
				owner->UpdateChildrenTransforms();
			};

			float scaleMult = scale.x;
			if (ImGui::DragFloat("Scale Multiplier", &scaleMult, 0.01f, 0.0f, 10.0f))
			{
				SetScale(float3(scaleMult, scaleMult, scaleMult));
				UpdateGlobalTransform();
				owner->UpdateChildrenTransforms();
			}

			ImGui::Spacing();

			if (ImGui::Button("Reset"))
			{
				Reset();
			}

			ImGui::Spacing();
		}
	}
}

// Setters and getters ------------------------------------------------------------------------

float4x4 Transform::GetGlobalTransform()
{
	return gTransform;
}

float4x4 Transform::GetLocalTransform()
{
	return lTransform;
}

void Transform::SetPos(float x, float y, float z)
{
	pos.Set(x, y, z);
}
void Transform::SetPos(float3 p)
{
	pos = p;
}
float3 Transform::GetPos()
{
	return pos;
}
void Transform::SetRot(float x, float y, float z)
{
	rot = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);
}
void Transform::SetRot(Quat q)
{
	rot = q;
}
Quat Transform::GetRot()
{
	return rot;
}
void Transform::SetScale(float x, float y, float z)
{
	scale.Set(x, y, z);
}
void Transform::SetScale(float3 s)
{
	scale = s;
}
float3 Transform::GetScale()
{
	return scale;
}