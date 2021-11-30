#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Globals.h"
#include "imgui.h"

//JSON
#include "prettywriter.h"
#include "document.h"

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> JSONWriter;
typedef rapidjson::Value JSONReader;

class GameObject;

enum class ComponentTypes
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
	//virtual void OnLoad(const JSONReader& reader);
	//virtual void OnSave(JSONWriter& writer) const;

public:
	ComponentTypes type = ComponentTypes::NOT_ASSIGNED;
	GameObject* owner = nullptr;

protected:
	bool active = true;
};

#endif // !__COMPONENT_H__