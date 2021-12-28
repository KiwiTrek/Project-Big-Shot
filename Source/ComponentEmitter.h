#ifndef __COMPONENT_EMITTER_H__
#define __COMPONENT_EMITTER_H__

#include "Globals.h"
#include <string>
#include "MathGeoLib.h"

class GameObject;

class ComponentEmitter : public Component
{
public:
	ComponentEmitter(bool active = true);
	~ComponentEmitter();

	void Update(float dt, Application* App) override;
	void DrawInspector(Application* App);

public:

};

#endif //!__COMPONENT_EMITTER_H__