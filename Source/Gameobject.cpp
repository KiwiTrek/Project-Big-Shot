#include "Globals.h"
#include "RenderGlobals.h"
#include "MathGeoLib.h"
#include "ModuleGameObjects.h"
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

	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		delete* c;
	}
	components.clear();
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
	case ComponentTypes::EMITTER:
	{
		EmitterData data;
		c = new ComponentEmitter(data);
		break;
	}
	case ComponentTypes::BILLBOARD:
	{
		c = new ComponentBillboard();
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

void GameObject::OnLoad(const jsonObject& reader, Application* App)
{
	bool isRoot = false;
	if (reader.HasMember("UID")) uid = reader["UID"].GetInt();

	if (reader.HasMember("name"))
	{
		name = reader["name"].GetString();
		if (name == "Main") isRoot = true;
	}

	if (reader.HasMember("active")) active = reader["active"].GetBool();

	if (reader.HasMember("Components"))
	{
		const jsonObject& jComponents = reader["Components"].GetObjectJSON();
		if (jComponents.HasMember("Transform"))
		{
			ComponentTransform* ct = GetComponent<ComponentTransform>();
			ct->OnLoad(jComponents["Transform"], App);
		}
		if (jComponents.HasMember("Mesh"))
		{
			ComponentMesh* cm = (ComponentMesh*)CreateComponent(ComponentTypes::MESH);
			cm->OnLoad(jComponents["Mesh"], App);
		}
		if (jComponents.HasMember("Material"))
		{
			ComponentMaterial* cmat = (ComponentMaterial*)CreateComponent(ComponentTypes::MATERIAL);
			cmat->OnLoad(jComponents["Material"], App);
		}
		if (jComponents.HasMember("Camera"))
		{
			ComponentCamera* cc = (ComponentCamera*)CreateComponent(ComponentTypes::CAMERA);
			cc->OnLoad(jComponents["Camera"], App);
		}
		components.size();
	}

	if (reader.HasMember("Children"))
	{
		const rapidjson::Value& jChildren = reader["Children"];
		for (rapidjson::Value::ConstValueIterator it = jChildren.Begin(); it != jChildren.End(); ++it)
		{
			const jsonObject& childOb = it->GetObjectJSON();
			const char* childName = "Default Game Object";
			if (childOb.HasMember("name")) childName = childOb["name"].GetString();

			GameObject* child = new GameObject(childName);
			if (isRoot)
			{
				App->gameObjects->AddGameobject(child);
			}
			else
			{
				this->AddChild(child);
				child->parent = this;
				child->parent->UpdateChildrenTransforms();
			}
			child->OnLoad(childOb, App);
		}
	}
}

void GameObject::OnSave(JSONWriter& writer) const
{
	writer.StartObject();
	writer.String("UID"); writer.Int(uid);
	writer.String("name"); writer.String(name.c_str());
	writer.String("active"); writer.Bool(active);
	if (components.size() != 0)
	{
		writer.String("Components");
		writer.StartObject();
		for (uint i = 0; i < components.size(); i++)
		{
			components[i]->OnSave(writer);
		}
		writer.EndObject();
	}
	if (children.size() != 0)
	{
		writer.String("Children");
		writer.StartArray();
		for (uint i = 0; i < children.size(); i++)
		{
			children[i]->OnSave(writer);
		}
		writer.EndArray();
	}
	writer.EndObject();
}