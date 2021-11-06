#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"

class GameObject;

class PanelHierarchy :public Panel
{
public:
	PanelHierarchy(Application* app, bool startEnabled = true);
	~PanelHierarchy();

	UpdateStatus Update();
	void DisplayChild(GameObject* g);
	bool RightClickMenu();
};

#endif // !__PANEL_HIERARCHY_H__