#ifndef __COMPONENT_BILLBOARD_H__
#define __COMPONENT_BILLBOARD_H__

#include "Globals.h"
#include <string>
#include "MathGeoLib.h"

class GameObject;
enum class BillboardType
{
	NONE,
	SCREEN,
	WORLD,
	AXIALY,
};

class ComponentBillboard : public Component
{
public:
	ComponentBillboard(bool active = true);
	~ComponentBillboard();

	void Update(float dt, Application* App) override;
	void DrawInspector(Application* App);

public:
	BillboardType bType = BillboardType::NONE;
private:
	bool xRot = false;
	bool yRot = true;
	bool zRot = false;
};

#endif //!__COMPONENT_BILLBOARD_H__