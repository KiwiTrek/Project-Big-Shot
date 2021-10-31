#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "imgui.h"

class GameObject;

enum ComponentTypes
{
	NOT_ASSIGNED,
	TRANSFORM,
	MESH,
	MATERIAL,
};

class Component
{
public:
	Component(ComponentTypes type, bool active = true) : type(ComponentTypes::NOT_ASSIGNED), active(active)
	{}

	virtual ~Component()
	{}

	virtual void Update()
	{}

	virtual void DrawInspector()
	{}

	virtual bool IsActive() { return active; }
	virtual void ToggleActive() { active = !active; }

public:
	ComponentTypes type;
	GameObject* owner;

protected:
	bool active;
};

#endif // !__COMPONENT_H__