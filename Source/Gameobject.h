#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"

#include <string>
#include <vector>

class Component;

class GameObject
{
public:
	GameObject(std::string n, bool active = true);
	~GameObject();
	void Update();

	Component* CreateComponent(ComponentTypes cType, MeshTypes mType = MeshTypes::NONE, Transform* t = nullptr);

	void CreateComponent(Component* c);
	bool RemoveComponent(Component* c);

	void AddChild(GameObject* c);
	int GetChildNum();
	GameObject* GetChildAt(int index);
	GameObject* GetParent();
	void SetParent(GameObject* p);
	bool RemoveChild(GameObject* gameObject);
	void DeleteChildren();
	void UpdateChildrenTransforms();

	void SetAxis(bool value);
	void CreatePrimitive(MeshTypes type);

	Transform* GetTransform();

public:
	bool active;
	std::string name;
	std::vector<Component*> components;

	GameObject* parent;
	std::vector<GameObject*> children;
};

#endif // !__GAMEOBJECT_H__