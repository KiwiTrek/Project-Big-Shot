#include "RenderGlobals.h"
#include "Gameobject.h"

#include "Application.h"
#include "ModuleGameObjects.h"
#include "ModuleRenderer3D.h"

ComponentEmitter::ComponentEmitter(bool active) : Component(type, active)
{
    type = ComponentTypes::EMITTER;
}

ComponentEmitter::~ComponentEmitter()
{
}

void ComponentEmitter::Update(float dt, Application* App)
{
}

void ComponentEmitter::DrawInspector(Application* App)
{
}
