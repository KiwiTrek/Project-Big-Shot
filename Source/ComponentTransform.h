#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

class GameObject;

class ComponentTransform : public Component
{
public:
	ComponentTransform(bool active = true);
	ComponentTransform(float3 p, Quat r, float3 s, bool active = true);
	~ComponentTransform();

	void DrawInspector();
	void Reset();
	void UpdateGlobalTransform();
	void UpdateGlobalTransform(float4x4 pGlobalTransform);
	void UpdateLocalTransform();

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

	//void OnLoad(const JSONReader& reader);
	//void OnSave(JSONWriter& writer) const;

private:
	float3 pos;
	Quat rot;
	float3 eulerRot;
	float3 scale;
	float4x4 gParentTransform;
	float4x4 gTransform;
	float4x4 lTransform;
};

#endif // !__COMPONENT_TRANSFORM_H__