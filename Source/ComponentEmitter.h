#ifndef __COMPONENT_EMITTER_H__
#define __COMPONENT_EMITTER_H__

#include "Globals.h"
#include <string>
#include <vector>
#include "MathGeoLib.h"

#include "Particle.h"

class GameObject;

struct EmitterData
{
	enum class EmitterType
	{
		NONE = -1,
		SMOKE,
		FIREWORK_PROJECTILE,
		FIREWORK_EXPLOSION
	} eType;

	enum class EmitterSphere
	{
		RANDOM,
		CENTER,
		BORDER
	} sType;

	float duration = 1.0f;
	bool loop = true;

	bool burst = false;
	int minPart = 0;
	int maxPart = 10;
	float repeatTime = 1.0f;

	float gravity = 0.0f;
	float3 particleDirection = float3::unitY;

	AABB cubeCreation = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));
	Sphere sphereCreation = Sphere(float3::zero, 1.0f);
	Circle circleCreation = Circle(float3::unitY, float3::unitY, 1.0f);

	Shape shapeType = Shape::CUBE;

	float3 sizeOBB = float3::zero;
	bool drawAABB = false;
	float3 posDifAABB = float3::zero;

	ResourceMesh* plane = nullptr;
	ResourceMaterial* texture = nullptr;
	int textureRows = 1;
	int textureColumns = 1;

	bool checkLife = false;
	bool checkSpeed = false;
	bool checkAcceleration = false;
	bool checkSize = false;
	bool checkSizeOverTime = false;
	bool checkRotation = false;
	bool checkAngularAcceleration = false;
	bool checkAngularVelocity = false;

	bool isSubEmitter = false;
	GameObject* subEmitter = nullptr;
	uint subEmitterUUID = 0u;

	int rateOverTime = 10;
	float timeToParticle = 0.0f;
	bool subEmitterActive = false;

	float2 particleLife = float2(5.0f, 5.0f);
	float2 speed = float2(3.0f, 3.0f);
	float2 acceleration = float2(0.0f, 0.0f);
	float2 sizeOverTime = float2(0.0f, 0.0f);
	float2 size = float2(1.0f, 1.0f);
	float2 rotation = float2(0.0f, 0.0f);
	float2 angularAcceleration = float2(0.0f, 0.0f);
	float2 angularVelocity = float2(0.0f, 0.0f);

	std::vector<FadeColor> color;
	bool timeColor = false;
};

class ComponentEmitter : public Component
{
public:
	ComponentEmitter(EmitterData data, bool active = true);
	~ComponentEmitter();

	void Update(float dt, Application* App) override;
	void PostUpdate();
	void DrawBbox() const;
	void DrawInspector(Application* App);
	void ClearEmitter();

	float GenerateRandNum(float min, float max);
	void ShowFloatValue(float2& value, bool checkBox, const char* name, float v_speed, float v_min, float v_max);

	void CreateParticles(int num, const float3& pos, int lastUsedParticle);
	void DestroyParticle(Particle* p);
	bool EditColor(FadeColor& color, uint pos = 0);

public:
	Timer lifeTimer;
	Timer burstTimer;
	Timer loopTimer;

	EmitterData data;
	uint maxParticles = MAX_PARTICLES;

	std::vector<Particle*> particlePool;
	Particle allParticles[MAX_PARTICLES];

	AABB bbox;
	AABB drawingBbox;
};

#endif //!__COMPONENT_EMITTER_H__