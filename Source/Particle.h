#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Globals.h"
#include "MathGeoLib.h"

class ResourceMesh;
class ResourceTexture;
class ComponentEmitter;

struct ParticleData
{
	uint maxLife;

	double rotSpeed;
	float initialSpeed;
	float finalSpeed;
	float3 initialSize;
	float3 finalSize;

	float2 randAngle;
	float3 randFinalSize;
	float3 randInitialSize;

	float4 initialColor;
	float4 finalColor;

	ResourceMesh* plane = nullptr;
	ResourceTexture* texture = nullptr;
};

class Particle
{
public:
	// Constructor
	Particle();

	// Called each loop iteration
	void Update(float dt);

	// Draws the particle
	bool Draw();

	float4x4 GetMatrix() const { return float4x4::FromTRS(pos, rot, scale).Transposed(); }

public:
	float camDistance = 0.0f;
	bool active = true;

	ComponentEmitter* owner = nullptr;

private:
	ParticleData data;

	float3 pos;
	Quat rot;
	float3 scale;
	float4 speed;

	uint life = 0;
};

struct particleCompare
{
	bool operator()(const Particle* particle1, const Particle* particle2) const
	{
		return particle1->camDistance > particle2->camDistance;
	}
};

#endif // !__PARTICLE_H__