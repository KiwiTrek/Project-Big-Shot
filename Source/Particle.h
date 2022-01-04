#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Globals.h"
#include "MathGeoLib.h"
#include "Color.h"

class ResourceMesh;
class ResourceMaterial;
class ComponentEmitter;

class Particle
{
public:
	// Constructor
	Particle(ResourceMesh* mesh, ResourceMaterial* tex, float lifeTime, float3 pos, float3 scale, float angle, float acc, float vel,float3 direction, float incrementSize, float angularAcc, float angularVel, std::vector<FadeColor> colors);
	Particle();
	~Particle();

	// Called each loop iteration
	bool Update(float dt);

	// Draws the particle
	void Draw();

	float4x4 GetMatrix();
public:
	float camDistance = 0.0f;
	bool active = false;

	ComponentEmitter* owner = nullptr;

private:
	float life = 0.0f;
	float maxLife = 0.0f;

	float3 pos = float3(0.0f,0.0f,0.0f);
	Quat rot = Quat::FromEulerXYZ(0,0,0);
	float3 scale = float3(0.0f,0.0f,0.0f);

	float acceleration = 0.0f;
	float speed = 0.0f;

	float angularAcceleration = 0.0f;
	float angularVelocity = 0.0f;

	float3 direction = float3(0.0f,0.0f,0.0f);
	float angle = 0.0f;

	float sizeOverTime = 0.0f;

	std::vector<FadeColor> color;
	int index = 0;
	bool multiColor = false;
	Color currentColor = Color();

	ResourceMesh* plane = nullptr;
	ResourceMaterial* texture = nullptr;
};

struct particleCompare
{
	bool operator()(const Particle* particle1, const Particle* particle2) const
	{
		return particle1->camDistance > particle2->camDistance;
	}
};

#endif // !__PARTICLE_H__