#include "ModuleParticles.h"
#include "Application.h"

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
