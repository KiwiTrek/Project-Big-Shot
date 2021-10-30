#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

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

	virtual void Enable()
	{}

	virtual void Disable()
	{}

public:
	ComponentTypes type;
	GameObject* owner;

private:
	bool active;
};

#endif // !__COMPONENT_H__