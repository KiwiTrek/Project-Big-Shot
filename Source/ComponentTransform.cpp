#include "Component.h"
#include "ComponentTransform.h"

Transform::Transform(bool active) : Component(type, active)
{
	type = ComponentTypes::TRANSFORM;

	pos = float3::zero;
	rot = Quat::identity;
	scale = float3::one;
	lTransform = float4x4::FromTRS(pos, rot, scale);
	gTransform = lTransform;
}

Transform::Transform(float3 p, Quat r, float3 s, bool active) : Component(type, active)
{
	type = ComponentTypes::TRANSFORM;

	pos = p;
	rot = r;
	scale = s;
	lTransform = float4x4::FromTRS(pos, rot, scale);
	gTransform = lTransform;
}

Transform::~Transform()
{
	pos.zero;
	rot.identity;
	scale.zero;
	lTransform.zero;
	gTransform.zero;
}

void Transform::Reset()
{
	pos = float3::zero;
	rot = Quat::identity;
	scale = float3::one;
	lTransform = float4x4::FromTRS(pos, rot, scale);
	gTransform = lTransform;
}

void Transform::UpdateGlobalTransform(float4x4 parent)
{
	lTransform = float4x4::FromTRS(pos, rot, scale);
	gTransform = parent * lTransform;
}

void Transform::UpdateLocalTransform()
{
	lTransform = float4x4::FromTRS(pos, rot, scale);
	//if parent UpdateGlobalTransform(parent)
	//else
	gTransform = lTransform;
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
	UpdateLocalTransform();
}
void Transform::SetPos(float3 p)
{
	pos = p;
	UpdateLocalTransform();
}
float3 Transform::GetPos()
{
	return pos;
}
void Transform::SetRot(float x, float y, float z)
{
	rot = Quat::FromEulerXYX(x * DEGTORAD, y * DEGTORAD, z * DEGTORAD);
	UpdateLocalTransform();
}
void Transform::SetRot(Quat q)
{
	rot = q;
	UpdateLocalTransform();
}
Quat Transform::GetRot()
{
	return rot;
}
void Transform::SetScale(float x, float y, float z)
{
	scale.Set(x, y, z);
	UpdateLocalTransform();
}
void Transform::SetScale(float3 s)
{
	scale = s;
	UpdateLocalTransform();
}
float3 Transform::GetScale()
{
	return scale;
}