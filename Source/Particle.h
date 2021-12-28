#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Globals.h"
#include "MathGeoLib.h"

class ResourceMesh;

class Particle
{
public:
	// Constructor
	Particle(ResourceMesh* shape);

	// Called each loop iteration
	void Update(float dt);

	// Draws the particle
	bool Draw();

private:

	float3 pos;
	Quat rot;
	float4 curSpeed;
	float3 curSize;

	ResourceMesh* plane;

	uint life;
};

#endif // !__PARTICLE_H__