#include "Particle.h"
#include "ResourceMesh.h"

Particle::Particle(ResourceMesh* shape) : plane(shape)
{}

void Particle::Update(float dt)
{

}

bool Particle::Draw()
{
    return false;
}
