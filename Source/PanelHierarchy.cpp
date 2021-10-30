#include "PanelHierarchy.h"

#include "Application.h"
#include "ModuleGuiManager.h"
#include "ModuleScene.h"

#include "Gameobject.h"
#include "Component.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

PanelHierarchy::PanelHierarchy(Application* app, bool start_enabled) : Panel(app, start_enabled)
{
    name = "Hierarchy";
}

PanelHierarchy::~PanelHierarchy()
{}

update_status PanelHierarchy::Update()
{
	ImGui::Begin(name.c_str());
	GameObject* root = App->scene->GetSceneRoot();
	DisplayChild(root);
	ImGui::End();

    return update_status::UPDATE_CONTINUE;
}

void PanelHierarchy::DisplayChild(GameObject* g)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (g->GetChildNum() > 0)
	{
		if (g == App->scene->GetSceneRoot()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
		if (g == App->scene->selectedGameObject) flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(g->name.c_str(), flags))
		{
			if (ImGui::IsItemClicked())
			{
				App->scene->selectedGameObject = g;
			}

			for (int i = 0; i < g->GetChildNum(); i++)
			{
				DisplayChild(g->GetChildAt(i));
			}
			ImGui::TreePop();
		}
	}
	else
	{
		flags |= ImGuiTreeNodeFlags_Leaf;

		if (g == App->scene->selectedGameObject) flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(g->name.c_str(), flags))
		{
			if (ImGui::IsItemClicked())
			{
				App->scene->selectedGameObject = g;
			}
			ImGui::TreePop();
		}
	}
}
