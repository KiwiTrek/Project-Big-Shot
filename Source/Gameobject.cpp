#include "Globals.h"
#include "RenderGlobals.h"
#include "Gameobject.h"

Gameobject::Gameobject(std::string n, bool active) : active(active)
{
	name = n;
	CreateComponent(ComponentTypes::TRANSFORM);
}

Gameobject::~Gameobject()
{
	name.clear();

	std::vector<Component*>::iterator c = components.begin();
	while (c != components.end())
	{
		delete *c;
		++c;
	}
}

void Gameobject::Update()
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

Component* Gameobject::CreateComponent(ComponentTypes t, MeshTypes mType)
{
	Component* c = nullptr;
	switch (t)
	{
	case TRANSFORM:
	{
		c = new Transform();
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

void Gameobject::CreateComponent(Component* c)
{
	c->owner = this;
	components.push_back(c);
}

void Gameobject::SetAxis(bool value)
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

void Gameobject::CreatePrimitive(MeshTypes type)
{
	if (type != MeshTypes::Primitive_Sphere || type != MeshTypes::Primitive_Grid)
	{
		CreateComponent(ComponentTypes::MATERIAL);
	}
	Mesh* m = (Mesh*)CreateComponent(ComponentTypes::MESH, type);
	if (type != MeshTypes::Primitive_Sphere || type != MeshTypes::Primitive_Grid)
	{
		m->GenerateBuffers();
	}
}

Transform* Gameobject::GetTransform()
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