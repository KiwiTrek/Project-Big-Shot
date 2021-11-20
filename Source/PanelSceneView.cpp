#include "PanelSceneView.h"
#include "Application.h"
#include "ModuleGuiManager.h"

#include "ModuleViewportFrameBuffer.h"
#include "ModuleGameObjects.h"
#include "ModuleCamera3D.h"

PanelSceneView::PanelSceneView(Application* app, bool startEnabled) : Panel(app, startEnabled)
{
	name = "SceneView";
}

PanelSceneView::~PanelSceneView()
{}

UpdateStatus PanelSceneView::Update()
{
    ImGui::Begin("Scene", &active, ImGuiWindowFlags_NoScrollbar);

    ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
    if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
    {
        //TODO: Modify Camera
        //App->camera->aspectRatio = viewportSize.x / viewportSize.y;
        //App->camera->RecalculateProjection();
    }
    lastViewportSize.x = viewportSize.x;
    lastViewportSize.y = viewportSize.y;
    ImGui::Image((ImTextureID)App->viewportBuffer->texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
	return UpdateStatus::UPDATE_CONTINUE;
}