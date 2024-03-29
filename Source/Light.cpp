#include "RenderGlobals.h"
#include "Light.h"

Light::Light() : ref(-1), on(false), position(float3(0.0f, 0.0f, 0.0f))
{}

void Light::Init()
{
	GLfloat ambientGl[] = { ambient.x,ambient.y,ambient.z,ambient.w };
	GLfloat diffuseGl[] = { diffuse.x,diffuse.y,diffuse.z,diffuse.w };

	glLightfv(ref, GL_AMBIENT, ambientGl);
	glLightfv(ref, GL_DIFFUSE, diffuseGl);
}

void Light::SetPos(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Light::Render()
{
	if (on)
	{
		float pos[] = { position.x, position.y, position.z, 1.0f };
		glLightfv(ref, GL_POSITION, pos);
	}
}

void Light::Active(bool active)
{
	if (on != active)
	{
		on = !on;
		on ? glEnable(ref) : glDisable(ref);
	}
}