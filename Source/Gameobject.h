#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include <string>
#include <vector>

class Component;
typedef ComponentMaterial Material;
typedef ComponentMesh Mesh;
typedef ComponentTransform Transform;
typedef ComponentCamera Camera;

typedef rapidjson::GenericObject<true, rapidjson::Value> jsonObject;

class GameObject
{
public:
	GameObject(std::string n, bool active = true);
	~GameObject();

	void Update();
	bool CleanUp();

	Component* CreateComponent(ComponentTypes cType, ComponentTransform* t = nullptr);
	void CreateComponent(Component* c);
	bool RemoveComponent(Component* c);

	void AddChild(GameObject* c);
	int GetChildNum();
	GameObject* GetChildAt(int index);
	bool RemoveChild(GameObject* gameObject);
	void DeleteChildren(bool isOriginal = true);
	void UpdateChildrenTransforms();

	void OnLoad(const jsonObject& reader, Application* App);
	void OnSave(JSONWriter& writer) const;

	template<class T> T* GetComponent()
	{
		T* component = nullptr;
		std::vector<Component*>::iterator c = components.begin();
		while (c != components.end())
		{
			component = dynamic_cast<T*>((*c));
			if (component)
				break;
			c++;
		}
		return component;
	}

public:
	bool active;
	std::string name;
	uint uid;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	GameObject* parent;
};

#endif // !__GAMEOBJECT_H__