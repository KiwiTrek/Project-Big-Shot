#ifndef __PANEL_ABOUT_H__
#define __PANEL_ABOUT_H__

#include "Panel.h"

class PanelAbout :public Panel
{
public:
    PanelAbout(Application* app, bool start_enabled = false);
    ~PanelAbout();

    update_status Update();
};

#endif // !__PANEL_ABOUT_H__