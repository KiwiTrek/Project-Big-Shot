#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Panel.h"

class PanelConsole :public Panel
{
public:
    PanelConsole(Application* app, bool start_enabled = true);
    ~PanelConsole();

    update_status Update();
};

#endif // !__PANEL_CONSOLE_H__