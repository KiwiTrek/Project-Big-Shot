#ifndef __PANEL_CONFIG_H__
#define __PANEL_CONFIG_H__

#include "Panel.h"

class PanelConfig :public Panel
{
public:
    PanelConfig(Application* app, bool start_enabled = false);
    ~PanelConfig();

    update_status Update();
    bool CleanUp();
    void UpdateHistogram();

public:
    std::vector<float> fpsHist;
    std::vector<float> msHist;
};

#endif // !__PANEL_CONFIG_H__