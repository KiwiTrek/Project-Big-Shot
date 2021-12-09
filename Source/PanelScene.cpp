#include "PanelScene.h"
#include "Application.h"
#include "ModuleGuiManager.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Scene", &active, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove))
	{
		App->gui->scenePanelOrigin = ImGui::GetWindowPos();
		App->gui->scenePanelOrigin.x += ImGui::GetWindowContentRegionMin().x;
		App->gui->scenePanelOrigin.y += ImGui::GetWindowContentRegionMin().y;

		int winX, winY;
		App->window->GetPosition(winX, winY);
		App->gui->scenePanelOrigin.x -= winX;
		App->gui->scenePanelOrigin.y -= winY;

		App->gui->mouseScenePosition.x = App->input->GetMouseX() - App->gui->scenePanelOrigin.x;
		App->gui->mouseScenePosition.y = App->input->GetMouseY() - App->gui->scenePanelOrigin.y;

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		if (viewportSize.x != lastViewportSize.x || viewportSize.y != lastViewportSize.y)
		{
			lastViewportSize = viewportSize;
			App->camera->aspectRatio = viewportSize.x / viewportSize.y;
			App->camera->RecalculateProjection();
			App->viewport->OnResize(viewportSize.x, viewportSize.y);
		}
		App->gui->viewportSize = viewportSize;
		ImGui::Image((ImTextureID)App->viewport->texture, viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::End();
	}
	ImGui::PopStyleVar();
	
	return UpdateStatus::UPDATE_CONTINUE;
}