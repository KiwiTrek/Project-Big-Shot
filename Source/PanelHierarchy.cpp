#include "PanelHierarchy.h"

#include "Application.h"
#include "ModuleGuiManager.h"
#include "ModuleGameObjects.h"

#include "Gameobject.h"

PanelHierarchy::PanelHierarchy(Application* app, bool start_enabled) : Panel(app, start_enabled)
{
    name = "Hierarchy";
}

PanelHierarchy::~PanelHierarchy()
{}

update_status PanelHierarchy::Update()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	GameObject* root = App->scene->GetSceneRoot();
	DisplayChild(root);





	//if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_DOWN)
	//{
		//TODO: Right Click to hierarchy options
		RightClickMenu();
	//}






	if (App->gameObjects->selectedGameObject != nullptr && ImGui::IsWindowFocused())
	{


		if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_STATE::KEY_DOWN)
		{
			App->gameObjects->RemoveGameobject(App->gameObjects->selectedGameObject);
			App->gameObjects->selectedGameObject = nullptr;
		}
	}

	ImGui::End();

    return update_status::UPDATE_CONTINUE;
}

void PanelHierarchy::DisplayChild(GameObject* g)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	if (g->GetChildNum() > 0)
	{
		if (g == App->scene->GetSceneRoot()) flags |= ImGuiTreeNodeFlags_DefaultOpen;
		if (g == App->gameObjects->selectedGameObject) flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(g->name.c_str(), flags))
		{
			if (ImGui::IsItemClicked())
			{
				App->gameObjects->selectedGameObject = g;
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

		if (g == App->gameObjects->selectedGameObject) flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(g->name.c_str(), flags))
		{
			if (ImGui::IsItemClicked())
			{
				App->gameObjects->selectedGameObject = g;
			}
			ImGui::TreePop();
		}
	}
}

bool PanelHierarchy::RightClickMenu()
{
	/*TODO: Popup menu: (FOR SOME REASON IT DOESNT WORK ;;)
	* - Move Up/Down in hierarchy
	* - Delete (Same as pressing DELETE)
	* - Create Empty GameObject child
	* (If it doesnt work, create menu as "Options" in config -> Lin 203)
	*/

	if (App->gameObjects->selectedGameObject != nullptr && App->gameObjects->selectedGameObject != App->scene->GetSceneRoot())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Move Up"))
			{
				std::vector<GameObject*> list = App->gameObjects->selectedGameObject->parent->children;
				std::vector<GameObject*>::iterator listItem = list.begin();
				while (listItem != list.end())
				{
					if ((*listItem) == App->gameObjects->selectedGameObject)
					{
						if ((*listItem) != list.front())
						{
							GameObject* tmp = (*listItem);
							listItem = list.erase(listItem);
							list.insert(--listItem, tmp);
							App->gameObjects->selectedGameObject->parent->children = list;
							break;
						}
						else
						{
							break;
						}
					}
					listItem++;
				}
			}
			if (ImGui::MenuItem("Move Down"))
			{
				std::vector<GameObject*> list = App->gameObjects->selectedGameObject->parent->children;
				std::vector<GameObject*>::iterator listItem = list.begin();
				while (listItem != list.end())
				{
					if ((*listItem) == App->gameObjects->selectedGameObject)
					{
						if ((*listItem) != list.back())
						{
							GameObject* tmp = (*listItem);
							listItem = list.erase(listItem);
							list.insert(++listItem, tmp);
							App->gameObjects->selectedGameObject->parent->children = list;
							break;
						}
						else
						{
							break;
						}
					}
					listItem++;
				}
			}
			if (ImGui::MenuItem("Delete"))
			{
				App->gameObjects->RemoveGameobject(App->gameObjects->selectedGameObject);
				App->gameObjects->selectedGameObject = nullptr;
			}
			if (ImGui::MenuItem("Create Empty"))
			{
				// TODO: if you are adding the empty at the last element of the hierarchy it doesnt open the menu
				GameObject* g = new GameObject("Empty GameObject");
				App->gameObjects->selectedGameObject->AddChild(g);
				App->gameObjects->selectedGameObject = g;
			}
			ImGui::EndMenu();
		}
	}



	//ImGui::OpenPopup("AAAAAAAAAAAA");
	//ImGui::SameLine();
	//if (ImGui::BeginPopup("AAAAAAAAAAAA"))
	//{
	//	ImGui::Text("This is a sample");
	//	ImGui::Separator();
	//	ImGui::Text("This is another sample :)");
	//	ImGui::EndPopup();
	//}


	return true;
}
