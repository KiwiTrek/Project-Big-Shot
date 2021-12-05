#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__

#include "Panel.h"
#include "imgui.h"

class PanelScene :public Panel
{
public:
	PanelScene(Application* app, bool startEnabled = true);
	~PanelScene();

	UpdateStatus Update();

private:
	ImVec2 lastViewportSize;
};

#endif // !__PANEL_SCENE_H__

