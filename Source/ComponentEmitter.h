#ifndef __COMPONENT_EMITTER_H__
#define __COMPONENT_EMITTER_H__

#include "Globals.h"
#include <string>
#include <vector>
#include "MathGeoLib.h"

#include "Particle.h"

struct EmitterData
{
	enum class EmitterType
	{
		NONE = -1,
		SMOKE,
		FIREWORK_PROJECTILE,
		FIREWORK_EXPLOSION
	} eType;

	uint maxLife;

	double rotSpeed;
	float initialSpeed;
	float finalSpeed;
	float3 initialSize;
	float3 finalSize;

	Color initialColor;
	Color finalColor;

	uint emitNumber;
	uint emitNumMult;
	uint maxParticleLife;
	double emitLifetime;
	float3 angleRange;

	float2 randRotSpeed;
	float2 randInitialSpeed;
	float2 randFinalSpeed;
	float2 randEmitMult;
	float2 randLife;
	float2 randInitialSize;
	float2 randFinalSize;
};

class ComponentEmitter : public Component
{
public:
	ComponentEmitter(EmitterData data, bool active = true);
	~ComponentEmitter();

	void Update(float dt, Application* App) override;
	void DrawInspector(Application* App);

	float GenerateRandNum(float min, float max);

	void DestroyParticle(Particle* p);

public:
	Timer lifeTimer;
	EmitterData data;
	uint maxParticles;

	std::vector<Particle*> particlePool;
};

#endif //!__COMPONENT_EMITTER_H__