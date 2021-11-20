#ifndef __PANEL_SCENE_VIEW_H__
#define __PANEL_SCENE_VIEW_H__

#include "Panel.h"
#include "glmath.h"

class PanelSceneView :public Panel
{
public:
	PanelSceneView(Application* app, bool startEnabled = true);
	~PanelSceneView();

	UpdateStatus Update();
private:
	vec2 lastViewportSize;
};

#endif // !__PANEL_SCENE_VIEW_H__