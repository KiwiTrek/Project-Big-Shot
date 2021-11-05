#include "Globals.h"
#include "RenderGlobals.h"
#include "Gameobject.h"

GameObject::GameObject(std::string n, bool active) : active(active), parent(nullptr)
{
	name = n;
	CreateComponent(ComponentTypes::TRANSFORM);
}

GameObject::~GameObject()
{
	name.clear();
	parent = nullptr;

	std::vector<Component*>::iterator c = components.begin();
	while (c != components.end())
	{
		delete *c;
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

		//children update
	}
}

Component* GameObject::CreateComponent(ComponentTypes cType, MeshTypes mType, Transform* t)
{
	Component* c = nullptr;
	switch (cType)
	{
	case TRANSFORM:
	{
		if (this->GetTransform() != nullptr)
		{
			RemoveComponent(this->GetTransform());
		}

		if (t != nullptr)
		{
			c = t;
		}
		else
		{
			c = new Transform();
		}
		break;
	}
	case MESH:
	{
		switch (mType)
		{
		case MeshTypes::Primitive_Grid:
		{
			c = new Grid();
			break;
		}
		case MeshTypes::Primitive_Plane:
		{
			c = new PlaneP();
			break;
		}
		case MeshTypes::Primitive_Cube:
		{
			c = new CubeP();
			break;
		}
		case MeshTypes::Primitive_Sphere:
		{
			c = new SphereP();
			break;
		}
		case MeshTypes::Primitive_Cylinder:
		{
			c = new CylinderP();
			break;
		}
		case MeshTypes::Primitive_Pyramid:
		{
			c = new PyramidP();
			break;
		}
		default:
			break;
		}
		break;
	}
	case MATERIAL:
	{
		c = new Material();
		Material* m = (Material*)c;
		m->SetTexture(nullptr);
		break;
	}
	default:
		break;
	}

	if (c != nullptr)
	{
		c->owner = this;
		components.push_back(c);
	}
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
		if (components[i] == c) {
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
	Mesh* m = nullptr;
	std::vector<Component*>::iterator c = components.begin();
	while (c != components.end())
	{
		if ((*c)->type == ComponentTypes::MESH)
		{
			m = (Mesh*)(*c);
		}
		++c;
	}

	if (m != nullptr)
	{
		m->axis = value;
	}
}

void GameObject::CreatePrimitive(MeshTypes type)
{
	if (type != MeshTypes::Primitive_Sphere && type != MeshTypes::Primitive_Grid)
	{
		CreateComponent(ComponentTypes::MATERIAL);
	}
	Mesh* m = (Mesh*)CreateComponent(ComponentTypes::MESH, type);
	if (type != MeshTypes::Primitive_Sphere && type != MeshTypes::Primitive_Grid)
	{
		m->GenerateBuffers();
	}
}

Transform* GameObject::GetTransform()
{
	Transform* t = nullptr;
	std::vector<Component*>::iterator c = components.begin();
	while (c != components.end())
	{
		if ((*c)->type == ComponentTypes::TRANSFORM)
		{
			t = (Transform*)(*c);
		}
		++c;
	}
	return t;
}

void GameObject::UpdateChildrenTransforms()
{
	Transform* t = GetTransform();
	t->UpdateGlobalTransform();

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->GetTransform()->UpdateGlobalTransform(t->GetGlobalTransform());
		children[i]->UpdateChildrenTransforms();
	}
}

void GameObject::AddChild(GameObject* c)
{
	children.push_back(c);
	c->SetParent(this);
}

int GameObject::GetChildNum()
{
	return children.size();
}

GameObject* GameObject::GetChildAt(int index)
{
	return children[index];
}

GameObject* GameObject::GetParent()
{
	return parent;
}

void GameObject::SetParent(GameObject* p)
{
	parent = p;
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

	if (!isOriginal)
	{
		this->~GameObject();
	}
}

bool GameObject::CleanUp()
{
	DeleteChildren();

	return true;
}