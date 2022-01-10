#include "ModuleParticles.h"
#include "Application.h"

#include "ModuleInput.h"
#include "ModuleGameObjects.h"
#include "ModuleResources.h"
#include <algorithm>

ModuleParticles::ModuleParticles(Application* app, bool startEnabled): Module(app, startEnabled)
{}

ModuleParticles::~ModuleParticles()
{}

bool ModuleParticles::Init()
{
    LOG_CONSOLE("Initializing Particles handler");
    plane = (ResourceMesh*)App->resources->GetShape(Shape::PLANE);
    return true;
}

bool ModuleParticles::Start()
{
    plane->GenerateBuffers();

    firework = CreateEmitter(CreateFireworkData());
    firework->GetComponent<Transform>()->SetPos(float3(-5.0f, 1.0f, -5.0f));
    firework->name = "firework";
    App->gameObjects->AddGameobject(firework);

    return true;
}

UpdateStatus ModuleParticles::Update(float dt)
{
    if (App->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN)
    {
        if (firework)
        {
            LOG_CONSOLE("Created firework!");
        }
    }

    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
    {
        (*it)->GetComponent<Emitter>()->Update(dt, App);
    }

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleParticles::PostUpdate()
{
    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
    {
        Emitter* e = (*it)->GetComponent<Emitter>();
        GameObject* c = App->gameObjects->mainCamera;
        if ((*it)->active)
        {
            if (c->GetComponent<Camera>()->culling == true)
            {
                if (c->GetComponent<Camera>()->ContainsBBox(e->bbox))
                {
                    if (e->IsActive()) e->PostUpdate();
                    if ((*it) == App->gameObjects->selectedGameObject)
                    {
                        e->DrawBbox();
                    }
                }
            }
            else
            {
                if (e->IsActive()) e->PostUpdate();
                if ((*it) == App->gameObjects->selectedGameObject)
                {
                    e->DrawBbox();
                }
            }
        }
    }

    std::vector<Particle*> allParticles;
    int i = 0;
    for (std::vector<GameObject*>::iterator gIt = emitters.begin(); gIt != emitters.end(); ++gIt)
    {
        Emitter* e = (*gIt)->GetComponent<Emitter>();
        for (std::vector<Particle*>::iterator pIt = e->particlePool.begin(); pIt != e->particlePool.end(); ++pIt)
        {
            allParticles.push_back((*pIt));
            ++i;
        }
        if (i >= MAX_PARTICLES) break;
    }

    SortParticles(allParticles);

    for (std::vector<Particle*>::iterator it = allParticles.begin(); it != allParticles.end(); ++it)
    {
        if ((*it)->active && (*it)->owner != nullptr && (*it) != nullptr)
        {
            if (App->gameObjects->mainCamera->GetComponent<Camera>()->culling)
            {
                if (App->gameObjects->mainCamera->GetComponent<Camera>()->ContainsBBox((*it)->bbox))
                    (*it)->Draw();
            }
            else
            {
                (*it)->Draw();
            }
        }
    }

    allParticles.clear();

    return UpdateStatus::UPDATE_CONTINUE;
}
bool ModuleParticles::CleanUp()
{
    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); it)
    {
        it = DeleteEmitter((*it));
        if (emitters.empty()) break;
    }
    emitters.clear();

    plane = nullptr;
    if (firework != nullptr)
    {
        firework->RemoveComponent(firework->GetComponent<Emitter>());
        firework->CleanUp();
        delete firework;
        firework = nullptr;
    }
    return true;
}

GameObject* ModuleParticles::CreateEmitter(EmitterData data)
{
    GameObject* go = new GameObject("Emitter");
    if (data.color.empty())
    {
        data.color.push_back(FadeColor(Color(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, true));
        data.color.push_back(FadeColor(Color(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true));
    }
    if (data.plane == nullptr) data.plane = plane;
    Emitter* e = (Emitter*)go->CreateComponent(ComponentTypes::EMITTER, data);
    emitters.push_back(go);
    return go;
}

std::vector<GameObject*>::iterator ModuleParticles::DeleteEmitter(GameObject* e)
{
    Emitter* emitter = e->GetComponent<Emitter>();
    emitter->ClearEmitter();
    emitter->data.color.clear();
    emitter->data.texture = nullptr;
    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
    {
        if ((*it) == e)
        {
            return emitters.erase(it);
        }
    }
    return emitters.end();
}

void ModuleParticles::SortParticles(std::vector<Particle*> &particlePool)
{
    std::sort(particlePool.begin(), particlePool.end(), particleCompare());
}

EmitterData ModuleParticles::CreateFireworkData()
{
    EmitterData firework;

    firework.sType = EmitterData::EmitterSphere::CENTER;

    firework.duration = 3.0f;
    firework.loop = true;

    firework.burst = false;
    firework.minPart = 0;
    firework.maxPart = 10;
    firework.repeatTime = 1.0f;

    firework.gravity = 0.0f;
    firework.particleDirection = float3::unitY;

    firework.cubeCreation = AABB(float3(-0.5f, -0.5f, -0.5f), float3(0.5f, 0.5f, 0.5f));
    vec size = firework.cubeCreation.Size();
    size.x = 0.25;
    size.z = 0.25;
    firework.cubeCreation.SetFromCenterAndSize(firework.cubeCreation.CenterPoint(), size);
    firework.sphereCreation = Sphere(float3::zero, 1.0f);
    firework.circleCreation = Circle(float3::unitY, float3::unitY, 0.25f);
    firework.shapeType = Shape::CONE;

    firework.sizeOBB = float3::zero;
    firework.drawAABB = false;
    firework.posDifAABB = float3::zero;

    firework.subEmitterActive = true;

    firework.plane = App->particles->plane;
    firework.texture = (ResourceMaterial*)App->resources->RequestResource("firework_projectile.png");
    firework.texture->GenerateBuffers();
    firework.subTexture = (ResourceMaterial*)App->resources->RequestResource("firework_flare.png");
    firework.subTexture->GenerateBuffers();

    firework.checkLife = true;
    firework.checkSpeed = true;
    firework.checkAcceleration = true;
    firework.checkSize = true;
    firework.checkSizeOverTime = true;
    firework.checkRotation = false;
    firework.checkAngularAcceleration = true;
    firework.checkAngularVelocity = true;

    firework.rateOverTime = 3;
    firework.subRateOverTime = 10;
    firework.timeToParticle = 0.0f;

    firework.particleLife = float2(3.0f, 5.0f);
    firework.speed = float2(2.0f, 3.5f);
    firework.acceleration = float2(-0.3f, -0.4f);
    firework.sizeOverTime = float2(0.0f, 0.2f);
    firework.size = float2(1.0f, 1.20f);
    firework.rotation = float2(0.0f, 0.0f);
    firework.angularAcceleration = float2(0.0f, 0.0f);
    firework.angularVelocity = float2(0.0f, 0.0f);

    firework.color.push_back(FadeColor(Color(1.0f, 1.0f, 0.0f, 1.0f), 0.0f, true));
    firework.color.push_back(FadeColor(Color(1.0f, 0.0f, 0.0f, 0.7f), 1.0f, true));

    firework.subParticleLife = float2(7.0f, 10.0f);
    firework.subSpeed = float2(1.2f, 2.0f);
    firework.subAcceleration = float2(-0.1f, -0.2f);
    firework.subSizeOverTime = float2(0.0f, 0.2f);
    firework.subSize = float2(1.0f, 1.20f);
    firework.subRotation = float2(-360.0f, 360.0f);
    firework.subAngularAcceleration = float2(0.0f, 0.0f);
    firework.subAngularVelocity = float2(0.0f, 0.0f);

    firework.subColor.push_back(FadeColor(Color(1.0f, 1.0f, 0.0f, 1.0f), 0.0f, true));
    firework.subColor.push_back(FadeColor(Color(1.0f, 1.0f, 0.0f, 0.2f), 0.5f, true));
    firework.subColor.push_back(FadeColor(Color(1.0f, 1.0f, 0.0f, 0.0f), 1.0f, true));

    firework.timeColor = true;

    return firework;
}