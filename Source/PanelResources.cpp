#include "PanelResources.h"
#include "Application.h"
#include "ModuleGuiManager.h"

#include "ModuleInput.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

PanelResources::PanelResources(Application* app, bool startEnabled) : Panel(app, startEnabled)
{
	name = "Resources";
	flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
}

PanelResources::~PanelResources()
{
	flags = 0;
	selected = nullptr;
}

UpdateStatus PanelResources::Update()
{
	ImGui::Begin(name.c_str(), &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

	ImGui::BeginChild("Assets", ImVec2(ImGui::GetWindowContentRegionWidth() * 0.4f, ImGui::GetContentRegionAvail().y), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	{
		if (ImGui::IsWindowFocused() && selected != nullptr)
		{
			if (App->input->GetKey(SDL_SCANCODE_DELETE) == KeyState::KEY_DOWN)
			{
				App->resources->ReleaseResource(selected->GetUID());
				selected = nullptr;
			}
		}

		if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_DOWN && ImGui::IsAnyItemHovered() && ImGui::IsWindowHovered())
		{
			ImGui::OpenPopup("OptionsAssets");
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && ImGui::IsPopupOpen("OptionsAssets"))
		{
			ImGui::CloseCurrentPopup();
		}
		RightClickMenu();

		ImGuiTreeNodeFlags flagsLeaf = flags;
		flagsLeaf |= ImGuiTreeNodeFlags_Leaf;

		if (ImGui::TreeNodeEx("Materials", flags))
		{
			for (std::map<UID, Resource*>::iterator it = App->resources->resources.begin(); it != App->resources->resources.end(); ++it)
			{
				Resource* r = (*it).second;
				if (r->GetType() == Resource::Type::MATERIAL)
				{
					ResourceMaterial* rm = (ResourceMaterial*)r;
					if (r == selected)
					{
						flagsLeaf |= ImGuiTreeNodeFlags_Selected;
					}
					else
					{
						flagsLeaf &= ~(ImGuiTreeNodeFlags_Selected);
					}

					if (ImGui::TreeNodeEx(rm->name.c_str(), flagsLeaf))
					{
						if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1)) selected = r;
						if (ImGui::IsItemHovered())
						{
							if (App->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN) selected = r;
						}

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
						{
							int selectedUID = (int)selected->GetUID();
							ImGui::SetDragDropPayload("MATERIALS", &selectedUID, sizeof(int));
							ImGui::Text("%s", rm->name.c_str());
							if (rm->name == "Color_texture")
							{
								ImGui::SameLine();
								ImGui::Image((ImTextureID)rm->texId, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));
							}
							ImGui::EndDragDropSource();
						}

						ImGui::TreePop();
					}
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Meshes", flags))
		{
			for (std::map<UID, Resource*>::iterator it = App->resources->resources.begin(); it != App->resources->resources.end(); ++it)
			{
				Resource* r = (*it).second;
				if (r->GetType() == Resource::Type::MESH)
				{
					//ResourceMesh* rm = (ResourceMesh*)r;
					if (r == selected) flagsLeaf |= ImGuiTreeNodeFlags_Selected;
					else flagsLeaf &= ~(ImGuiTreeNodeFlags_Selected);

					if (ImGui::TreeNodeEx(std::to_string(r->GetUID()).c_str(), flagsLeaf))
					{
						if (ImGui::IsItemClicked() || ImGui::IsItemClicked(1)) selected = r;
						if (ImGui::IsItemHovered())
						{
							if (App->input->GetKey(SDL_SCANCODE_RETURN) == KeyState::KEY_DOWN) selected = r;
						}

						if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
						{
							int selectedUID = (int)selected->GetUID();
							ImGui::SetDragDropPayload("MESHES", &selectedUID, sizeof(int));
							ImGui::Text("%d", selectedUID);
							ImGui::EndDragDropSource();
						}

						ImGui::TreePop();
					}
				}
			}
			ImGui::TreePop();
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("Properties", ImVec2(0, ImGui::GetContentRegionAvail().y), true);
	{
		if (selected != nullptr)
		{
			IMGUI_PRINT(IMGUI_YELLOW, "UID: ", "%d", selected->GetUID());
			IMGUI_PRINT(IMGUI_YELLOW, "Assets File:", "%s", selected->GetAssetFile());
			IMGUI_PRINT(IMGUI_YELLOW, "Reference Count: ", "%d", selected->referenceCount);
			ImGui::Separator();
			switch (selected->GetType())
			{
			case Resource::Type::MATERIAL:
			{
				ResourceMaterial* material = (ResourceMaterial*)selected;
				IMGUI_PRINT(IMGUI_YELLOW, "Type: ", "Material");
				IMGUI_PRINT(IMGUI_YELLOW, "Name:", "%s", material->name.c_str());
				if (material->name == "Color_texture")
				{
					ImGui::SameLine();
					ImGui::Image((ImTextureID)material->texId, ImVec2(15, 15), ImVec2(0, 1), ImVec2(1, 0));
				}
				else
				{
					IMGUI_PRINT(IMGUI_YELLOW, "Path:", "%s", material->path.c_str());
				}
				IMGUI_PRINT(IMGUI_YELLOW, "Width:", "%d", material->width);
				IMGUI_PRINT(IMGUI_YELLOW, "Height:", "%d", material->height);
				break;
			}
			case Resource::Type::MESH:
			{
				ResourceMesh* mesh = (ResourceMesh*)selected;
				IMGUI_PRINT(IMGUI_YELLOW, "Type: ", "Mesh");
				IMGUI_PRINT(IMGUI_YELLOW, "Vertices:", "%d", mesh->vertexNum);
				IMGUI_PRINT(IMGUI_YELLOW, "Indices:", "%d", mesh->indices);
				break;
			}
			}
		}
		else
		{
			ImGui::Text("- Select a resource to see its properties -");
		}
	}
	ImGui::EndChild();

	ImGui::End();

	return UpdateStatus::UPDATE_CONTINUE;
}

bool PanelResources::RightClickMenu()
{
	if (ImGui::BeginPopupContextItem("OptionsAssets"))
	{
		if (ImGui::MenuItem("Delete"))
		{
			if (selected != nullptr)
			{
				App->resources->ReleaseResource(selected->GetUID());
				selected = nullptr;
			}
		}
		ImGui::EndPopup();
	}
	return true;
}