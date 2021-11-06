#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Panel.h"

class PanelConsole :public Panel
{
public:
	PanelConsole(Application* app, bool startEnabled = true);
	~PanelConsole();

	UpdateStatus Update();
public:
	//Update console
	bool update;
};

#endif // !__PANEL_CONSOLE_H__