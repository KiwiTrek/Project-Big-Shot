#include "Globals.h"
#include "RenderGlobals.h"
#include "MathGeoLib.h"
#include "Gameobject.h"

GameObject::GameObject(std::string n, bool active) : active(active), parent(nullptr)
{
	name = n;
	uid = LCG().Int();
	CreateComponent(ComponentTypes::TRANSFORM);
}

GameObject::~GameObject()
{
	name.clear();
	parent = nullptr;

	std::vector<Component*>::iterator c = components.begin();
	while (c != components.end())
	{
		delete* c;
		++c;
	}
	components.clear();
}

void GameObject::Update()
{
	if (!active)
	{
		std::vector<Component*>::iterator c = components.begin();
		while (c != components.end())
		{
			(*c)->Update();
			++c;
		}

		if (!children.empty())
		{
			std::vector<GameObject*>::iterator g = children.begin();
			while (g != children.end())
			{
				(*g)->Update();
				g++;
			}
		}
	}
}

Component* GameObject::CreateComponent(ComponentTypes cType, ComponentTransform* t)
{
	Component* c = nullptr;
	switch (cType)
	{
	case ComponentTypes::TRANSFORM:
	{
		if (this->GetComponent<Transform>() != nullptr) RemoveComponent(this->GetComponent<ComponentTransform>());

		t != nullptr ? c = t : c = new ComponentTransform();
		break;
	}
	case ComponentTypes::MESH:
	{
		c = new ComponentMesh();
		break;
	}
	case ComponentTypes::MATERIAL:
	{
		c = new ComponentMaterial();
		break;
	}
	case ComponentTypes::CAMERA:
	{
		c = new ComponentCamera();
		break;
	}
	default:
		break;
	}

	if (c != nullptr) CreateComponent(c);
	return c;
}

void GameObject::CreateComponent(Component* c)
{
	c->owner = this;
	components.push_back(c);
}

bool GameObject::RemoveComponent(Component* c)
{
	bool ret = false;

	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i] == c)
		{
			delete components[i];
			components.erase(components.begin() + i);
			c = nullptr;
			ret = true;
		}
	}

	return ret;
}

void GameObject::SetAxis(bool value)
{
	ComponentMesh* m = GetComponent<Mesh>();
	if (m != nullptr) m->axis = value;
}

void GameObject::UpdateChildrenTransforms()
{
	ComponentTransform* t = GetComponent<Transform>();
	t->UpdateGlobalTransform();

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->GetComponent<Transform>()->UpdateGlobalTransform(t->GetGlobalTransform());
		children[i]->UpdateChildrenTransforms();
	}
}

void GameObject::AddChild(GameObject* c)
{
	children.push_back(c);
	c->parent = this;
}

int GameObject::GetChildNum()
{
	return children.size();
}

GameObject* GameObject::GetChildAt(int index)
{
	return children[index];
}

bool GameObject::RemoveChild(GameObject* gameObject)
{
	bool ret = false;
	if (!children.empty())
	{
		for (size_t i = 0; i < children.size(); i++)
		{
			if (children[i] == gameObject)
			{
				children.erase(children.begin() + i);
				ret = true;
			}
		}
	}
	return ret;
}

void GameObject::DeleteChildren(bool isOriginal)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->DeleteChildren(false);
		children[i] = nullptr;
	}
	children.clear();

	if (!isOriginal) this->~GameObject();
}

bool GameObject::CleanUp()
{
	DeleteChildren();
	return true;
}

void GameObject::OnLoad(const JSONReader& reader)
{
	//if (reader.HasMember("Game Object"))
	//{
	//	const auto& go = reader["Game Object"];

	//}
}

void GameObject::OnSave(JSONWriter& writer) const
{
	//writer.StartObject();
	//writer.Int(uid);
	//writer.Int(parent->uid);
	//writer.String(name.c_str());
	//writer.Bool(active);
	//writer.String("Components");
	//writer.StartArray();
	//for (uint i = 0; i < components.size(); i++)
	//{
	//	components[i]->OnSave(writer);
	//}
	//writer.EndArray();
	//writer.String("Children");
	//writer.StartArray();
	//for (uint i = 0; i < children.size(); i++)
	//{
	//	children[i]->OnSave(writer);
	//}
	//writer.EndArray();
	//writer.EndObject();
}