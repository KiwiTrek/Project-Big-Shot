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
}