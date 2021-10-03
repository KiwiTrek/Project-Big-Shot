#pragma once
#include "Panel.h"

class PanelAbout :public Panel
{
public:
    PanelAbout(Application* app, bool start_enabled = false);
    ~PanelAbout();

    update_status Update();
};