#include "GameObject.h"
#include "RenderGlobals.h"

#include "Application.h"
#include "ModuleRenderer3D.h"

ComponentEmitter::ComponentEmitter(EmitterData data, bool active) : Component(type, active), data(data)
{
    type = ComponentTypes::EMITTER;

	if (this->data.emitLifetime != -1.0f && this->data.emitLifetime > 0.0f)
	{
		lifeTimer.Start();
	}

	particlePool.clear();
}

ComponentEmitter::~ComponentEmitter()
{
	particlePool.clear();
}

void ComponentEmitter::Update(float dt, Application* App)
{
	if (active)
	{
		// Particle generation
		int emissionMultiplier = (int)(data.emitNumber + data.emitNumMult * GenerateRandNum(data.randEmitMult.x, data.randEmitMult.y));

		for (int i = 1; i <= emissionMultiplier; i++)
		{
			ParticleData pData;

			pData.initialSpeed = data.initialSpeed * GenerateRandNum(data.randInitialSpeed.x, data.randInitialSpeed.y);
			pData.finalSpeed = data.finalSpeed * GenerateRandNum(data.randFinalSpeed.x, data.randFinalSpeed.y);
			
			pData.randAngle.x = GenerateRandNum(data.angleRange.x, data.angleRange.y);
			pData.randAngle.y = GenerateRandNum(data.angleRange.x, data.angleRange.y);
			
			float3 randInitialSize;
			randInitialSize.x = data.initialSize.x * GenerateRandNum(data.randInitialSize.x, data.randInitialSize.y);
			randInitialSize.y = data.initialSize.y * GenerateRandNum(data.randInitialSize.x, data.randInitialSize.y);
			randInitialSize.z = data.initialSize.z * GenerateRandNum(data.randInitialSize.x, data.randInitialSize.y);
			
			pData.randFinalSize.x = data.finalSize.x * GenerateRandNum(data.randFinalSize.x, data.randFinalSize.y);
			pData.randFinalSize.y = data.finalSize.y * GenerateRandNum(data.randFinalSize.x, data.randFinalSize.y);
			pData.randFinalSize.z = data.finalSize.z * GenerateRandNum(data.randFinalSize.x, data.randFinalSize.y);

			pData.initialSize.x = GenerateRandNum(randInitialSize.x, pData.randFinalSize.x);
			pData.initialSize.y = GenerateRandNum(randInitialSize.y, pData.randFinalSize.y);
			pData.initialSize.z = GenerateRandNum(randInitialSize.z, pData.randFinalSize.z);
			double randRotationSpeed = pData.rotSpeed * GenerateRandNum(data.randRotSpeed.x, data.randRotSpeed.y);

			//Particle* p = new Particle(owner->GetComponent<Transform>()->GetPos(), pRandInitialSpeed, pRandFinalSpeed, pRandAngle, randRotationSpeed, pStartingSize, data.finalSize, data.maxParticleLife, data.texRect, data.initialColor, data.finalColor);
			//particlePool.push_back(p);
		}
	}

	// Emission timing calculations
	if (data.emitLifetime > 0.0f)
	{
		if (lifeTimer.Read() >= data.emitLifetime)
		{
			active = false;
			data.emitLifetime = 0.0f;
		}
	}

	// Update all alive particles
	for (std::vector<Particle*>::iterator p = particlePool.begin(); p != particlePool.end(); p++)
	{
		(*p)->Update(dt);
	}
}

void ComponentEmitter::DrawInspector(Application* App)
{}

float ComponentEmitter::GenerateRandNum(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float result = random * diff;

	return min + result;
}

void ComponentEmitter::DestroyParticle(Particle* p)
{
	int i = -1;
	std::vector<Particle*>::iterator it = particlePool.begin();
	while (it != particlePool.end())
	{
		if ((*it) == p)
		{
			particlePool.erase(it);
			RELEASE(p);
			break;
		}
		it++;
	}
}