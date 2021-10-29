#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

class Gameobject;

class Transform : public Component
{
public:
	Transform(bool active = true);
	Transform(float3 p, Quat r, float3 s,bool active = true);
	~Transform();
	void Reset();
	void SetPos(float x, float y, float z);
	void SetPos(float3 p);
	float3 GetPos();
	void SetRot(float x, float y, float z);
	void SetRot(Quat q);
	Quat GetRot();
	void SetScale(float x, float y, float z);
	void SetScale(float3 s);
	float3 GetScale();
	float4x4 GetGlobalTransform();
	float4x4 GetLocalTransform();
	void UpdateGlobalTransform(float4x4 parent);
	void UpdateLocalTransform();

private:
	float3 pos;
	Quat rot;
	float3 scale;
	float4x4 gTransform;
	float4x4 lTransform;
};

#endif // !__COMPONENT_TRANSFORM_H__