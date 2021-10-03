#pragma once
#include "Panel.h"

class PanelConfig :public Panel
{
public:
    PanelConfig(Application* app, bool start_enabled = false);
    ~PanelConfig();

    update_status Update();
    void UpdateHistogram();

public:
    std::vector<float> fpsHist;
    std::vector<float> msHist;
};