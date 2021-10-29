#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"

#include <string>
#include <vector>

class Component;

class Gameobject
{
public:
	Gameobject(std::string n, bool active = true);
	~Gameobject();
	void Update();
	Component* CreateComponent(ComponentTypes t, MeshTypes mTypes = MeshTypes::NONE);
	void CreateComponent(Component* c);
	void SetAxis(bool value);
	void CreatePrimitive(MeshTypes type);
	Transform* GetTransform();

public:
	bool active;
	std::string name;
	std::vector<Component*> components;
};

#endif // !__GAMEOBJECT_H__