#ifndef __PANEL_ABOUT_H__
#define __PANEL_ABOUT_H__

#include "Panel.h"

class PanelAbout :public Panel
{
public:
	PanelAbout(Application* app, bool startEnabled = false);
	~PanelAbout();

	UpdateStatus Update();
};

#endif // !__PANEL_ABOUT_H__