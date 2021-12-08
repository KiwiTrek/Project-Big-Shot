#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "Warnings.h"
#include "Globals.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleResources.h"

//JSON
#include "prettywriter.h"
#include "document.h"

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> JSONWriter;
typedef rapidjson::Value JSONReader;

class Application;
class GameObject;

enum class ComponentTypes
{
	NOT_ASSIGNED,
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
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

	virtual void DrawInspector(Application* App)
	{}

	virtual bool IsActive() { return active; }
	virtual void ToggleActive() { active = !active; }
	virtual void OnLoad(const JSONReader& reader, Application* App) {}
	virtual void OnSave(JSONWriter& writer) const {}

public:
	ComponentTypes type = ComponentTypes::NOT_ASSIGNED;
	GameObject* owner = nullptr;

protected:
	bool active = true;
};

#endif // !__COMPONENT_H__