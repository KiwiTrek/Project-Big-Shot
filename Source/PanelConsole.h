#pragma once
#include "Panel.h"

class PanelConsole :public Panel
{
public:
    PanelConsole(Application* app, bool start_enabled = false);
    ~PanelConsole();

    update_status Update();
};