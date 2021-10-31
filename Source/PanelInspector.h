#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"

class GameObject;

class PanelInspector :public Panel
{
public:
    PanelInspector(Application* app, bool start_enabled = true);
    ~PanelInspector();

    update_status Update();
};

#endif // !__PANEL_INSPECTOR_H__