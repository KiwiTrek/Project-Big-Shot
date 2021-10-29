#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"

class Gameobject;

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
	Gameobject* owner;

private:
	bool active;
};

#endif // !__COMPONENT_H__