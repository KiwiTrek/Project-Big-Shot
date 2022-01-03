#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Globals.h"
#include "MathGeoLib.h"
#include "Color.h"

class ResourceMesh;
class ResourceTexture;
class ComponentEmitter;

class Particle
{
public:
	// Constructor
	Particle(ResourceMesh* mesh, ResourceTexture* tex, float3 pos, Quat rot, float3 scale);
	Particle();
	~Particle();

	// Called each loop iteration
	bool Update(float dt);

	// Draws the particle
	bool Draw();

	float4x4 GetMatrix() const;
	void EndParticle(bool& ret);

public:
	float camDistance = 0.0f;
	bool active = true;

	ComponentEmitter* owner = nullptr;

private:
	uint life = 0;
	uint maxLife;

	float3 pos;
	Quat rot;
	float angle;
	float3 scale;

	float speed;
	float angularVelocity;

	float acceleration;
	float angularAcceleration;

	float3 direction;

	float sizeOverTime;

	std::vector<FadeColor> color;
	int index;
	bool multiColor;
	Color currentColor;

	ResourceMesh* plane = nullptr;
	ResourceTexture* texture = nullptr;

	bool subEmitter;
};

struct particleCompare
{
	bool operator()(const Particle* particle1, const Particle* particle2) const
	{
		return particle1->camDistance > particle2->camDistance;
	}
};

#endif // !__PARTICLE_H__