#include "PanelScene.h"
#include "Application.h"
#include "ModuleGuiManager.h"

#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleViewportBuffer.h"

PanelScene::PanelScene(Application* app, bool startEnabled) : Panel(app, startEnabled)
{
	name = "Scene";
}

PanelScene::~PanelScene()
{}

UpdateStatus PanelScene::Update()
{
	ImGui::Begin("Scene", &active, ImGuiWindowFlags_NoScrollbar);

	ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
	if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
	{
		App->camera->aspectRatio = viewportSize.x / viewportSize.y;
		App->camera->RecalculateProjection();
	}
	lastViewportSize = viewportSize;
	ImGui::Image((ImTextureID)App->viewport->texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	
	return UpdateStatus::UPDATE_CONTINUE;
}