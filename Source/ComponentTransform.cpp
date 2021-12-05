#include "Gameobject.h"

ComponentTransform::ComponentTransform(bool active) : Component(type, active)
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

ComponentTransform::ComponentTransform(float3 p, Quat r, float3 s, bool active) : Component(type, active)
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

ComponentTransform::~ComponentTransform()
{
	pos.zero;
	rot.identity;
	eulerRot.zero;
	scale.one;

	lTransform.zero;
	gTransform.zero;
	gParentTransform.zero;
}

void ComponentTransform::Reset()
{
	pos = float3::zero;
	rot = Quat::identity;
	eulerRot = float3::zero;
	scale = float3::one;

	UpdateGlobalTransform();
	owner->UpdateChildrenTransforms();
}

void ComponentTransform::UpdateGlobalTransform()
{
	UpdateLocalTransform();
	gTransform = gParentTransform * lTransform;
}

void ComponentTransform::UpdateGlobalTransform(float4x4 pGlobalTransform)
{
	UpdateLocalTransform();
	gParentTransform = pGlobalTransform;
	gTransform = gParentTransform * lTransform;
}

void ComponentTransform::UpdateLocalTransform()
{
	lTransform = float4x4::FromTRS(pos, rot, scale);
	eulerRot = rot.ToEulerXYZ() * RADTODEG;
}

void ComponentTransform::DrawInspector(Application* App)
{
	if (owner->parent != nullptr && ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
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

		if (owner->GetComponent<Camera>() == nullptr)
		{
			float scale4f[4] = { scale.x, scale.y, scale.z, 1.0f };
			if (ImGui::DragFloat3("Scale", scale4f, 0.01f, -10000.0f, 10000.0f))
			{
				SetScale(scale4f[0], scale4f[1], scale4f[2]);
				UpdateGlobalTransform();
				owner->UpdateChildrenTransforms();
			}

			float scaleMult = scale.x;
			if (ImGui::DragFloat("Uni-Scale", &scaleMult, 0.01f, 0.0f, 10.0f))
			{
				SetScale(float3(scaleMult, scaleMult, scaleMult));
				UpdateGlobalTransform();
				owner->UpdateChildrenTransforms();
			}
			ImGui::Spacing();
		}

		if (ImGui::Button("Reset")) Reset();
		ImGui::Spacing();
	}
}

float4x4 ComponentTransform::GetGlobalTransform()
{
	return gTransform;
}

float4x4 ComponentTransform::GetLocalTransform()
{
	return lTransform;
}

void ComponentTransform::SetPos(float x, float y, float z)
{
	pos.Set(x, y, z);
	UpdateLocalTransform();
}

void ComponentTransform::SetPos(float3 p)
{
	pos = p;
	UpdateLocalTransform();
}

float3 ComponentTransform::GetPos()
{
	return pos;
}

void ComponentTransform::SetRot(float x, float y, float z)
{
	rot = Quat::FromEulerXYZ(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);
	UpdateLocalTransform();
}

void ComponentTransform::SetRot(Quat q)
{
	rot = q;
	UpdateLocalTransform();
}

Quat ComponentTransform::GetRot()
{
	return rot;
}

void ComponentTransform::SetScale(float x, float y, float z)
{
	scale.Set(x, y, z);
	UpdateLocalTransform();
}

void ComponentTransform::SetScale(float3 s)
{
	scale = s;
	UpdateLocalTransform();
}

void ComponentTransform::SetGlobalTransform(float4x4 transform)
{
	gTransform = transform;
	float4x4 inverseParentGlobal = gParentTransform;
	inverseParentGlobal.Inverse();
	lTransform = inverseParentGlobal * gTransform;

	UpdateGlobalTransform();
	owner->UpdateChildrenTransforms();
}

float3 ComponentTransform::GetScale()
{
	return scale;
}

//void ComponentTransform::OnLoad(const JSONReader& reader)
//{
//
//}
//
//void ComponentTransform::OnSave(JSONWriter& writer) const
//{
//
//}