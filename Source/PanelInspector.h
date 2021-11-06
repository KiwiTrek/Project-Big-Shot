#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"

class GameObject;

class PanelInspector :public Panel
{
public:
	PanelInspector(Application* app, bool startEnabled = true);
	~PanelInspector();

	UpdateStatus Update();
};

#endif // !__PANEL_INSPECTOR_H__