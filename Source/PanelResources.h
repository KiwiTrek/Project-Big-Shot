#ifndef __PANEL_RESOURCES_H__
#define __PANEL_RESOURCES_H__

#include "Panel.h"
#include "Resource.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"
#include <map>

class Resource;
typedef int ImGuiTreeNodeFlags;

class PanelResources :public Panel
{
public:
	PanelResources(Application* app, bool startEnabled = true);
	~PanelResources();

	UpdateStatus Update();
	bool RightClickMenu();

public:
	ImGuiTreeNodeFlags flags;
	Resource* selected = nullptr;
	bool replace = false;
};

#endif // !__PANEL_RESOURCES_H__