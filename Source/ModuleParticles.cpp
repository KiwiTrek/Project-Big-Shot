#include "ModuleParticles.h"
#include "Application.h"

#include "ModuleInput.h"
#include "ModuleGameObjects.h"

ModuleParticles::ModuleParticles(Application* app, bool startEnabled): Module(app, startEnabled)
{}

ModuleParticles::~ModuleParticles()
{}

bool ModuleParticles::Init()
{
    LOG_CONSOLE("Initializing Particles handler");
    return true;
}

bool ModuleParticles::Start()
{
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
    SortParticles();

    for (uint i = 0; i < MAX_PARTICLES; ++i)
    {
        if (particles[i].active)
        {
            particles[i].Draw();
        }
    }
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
    for (uint i = 0; i < MAX_PARTICLES; ++i)
    {
        particles[i].active = false;
        particles[i].owner = nullptr;
    }

    for (std::vector<GameObject*>::iterator it = emitters.begin(); it != emitters.end(); ++it)
    {
        DeleteEmitter((*it));
    }
    emitters.clear();

    delete plane;
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

GameObject* ModuleParticles::CreateEmitter()
{
    GameObject* go = new GameObject("Emitter");
    Emitter* e = (Emitter*)go->CreateComponent(ComponentTypes::EMITTER);
    e->allParticles = particles;
    emitters.push_back(go);
    return go;
}

void ModuleParticles::DeleteEmitter(GameObject* e)
{
    Emitter* emitter = e->GetComponent<Emitter>();
    emitter->ClearEmitter();
    for (std::vector<GameObject*>::iterator it; it != emitters.end(); ++it)
    {
        if ((*it) == e)
        {
            emitters.erase(it);
        }
    }
}

void ModuleParticles::SortParticles()
{
    uint swaps = 0;
    for (uint i = 0; i < MAX_PARTICLES; ++i)
    {
        if (i + 1 <= MAX_PARTICLES && particles[i].camDistance > particles[i + 1].camDistance)
        {
            Swap<Particle>(particles[i], particles[i + 1]);
            swaps++;
        }
    }
    if (swaps > 0) SortParticles();
}