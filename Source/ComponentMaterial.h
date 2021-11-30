#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Globals.h"
#include <string>

typedef unsigned char GLubyte;
class GameObject;
class ResourceMaterial;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(bool active = true);
	~ComponentMaterial();

	void DrawInspector();
	void BindTexture(bool usingCheckers);

	//void OnLoad(const JSONReader& reader);
	//void OnSave(JSONWriter& writer) const;

public:
	bool usingCheckers = false;
	uint checkersId = -1;

	ResourceMaterial* material;
};

#endif //!__COMPONENT_MATERIAL_H__