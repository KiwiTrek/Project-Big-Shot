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
    return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
    return true;
}
