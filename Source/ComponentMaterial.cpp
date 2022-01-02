#include "RenderGlobals.h"
#include "ResourceMaterial.h"
#include "ModuleResources.h"
#include "Gameobject.h"

ComponentMaterial::ComponentMaterial(bool active) : Component(type, active)
{
	type = ComponentTypes::MATERIAL;

	usingCheckers = false;
	material = nullptr;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (checkersId == -1) glGenTextures(1, &checkersId);
	glBindTexture(GL_TEXTURE_2D, checkersId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

ComponentMaterial::~ComponentMaterial()
{
	material = nullptr;
}

void ComponentMaterial::DrawInspector(Application* App)
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Material", &active);
		ImGui::SameLine();
		if (ImGui::Checkbox("Checkers", &usingCheckers)) BindTexture(usingCheckers);

		ImGui::Separator();

		if (material != nullptr)
		{
			if (material->data != nullptr)
			{
				ImGui::Text("UID: "); ImGui::SameLine();
				ImGui::Button(std::to_string(material->GetUID()).c_str(), ImVec2(ImGui::CalcItemWidth(), 20));
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag material from Resources Panel here to change it.");
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIALS"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int payloadUID = *(const int*)payload->Data;
						material = (ResourceMaterial*)App->resources->RequestResource((UID)payloadUID);
						usingCheckers = false;
						BindTexture(usingCheckers);
					}
					ImGui::EndDragDropTarget();
				}

				IMGUI_PRINT(IMGUI_YELLOW, "Diffuse Texture:", "%s", material->name.c_str());
				IMGUI_PRINT(IMGUI_YELLOW, "Path:", "%s", material->path.c_str());
				IMGUI_PRINT(IMGUI_YELLOW, "Width:", "%d", material->width); ImGui::SameLine(); IMGUI_PRINT(IMGUI_YELLOW, "Height:", "%d", material->height);
				ImGui::Text("Image:");
				ImGui::Image((ImTextureID)material->texId, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::Button("Remove material", ImVec2(ImGui::CalcItemWidth(), 20)))
				{
					material->referenceCount--;
					material = nullptr;
				}
			}
		}
		else
		{
			ImGui::Text("UID: "); ImGui::SameLine();
			ImGui::Button("- None -", ImVec2(ImGui::CalcItemWidth(), 20));
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag material from Resources Panel here to change it.");
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIALS"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payloadUID = *(const int*)payload->Data;
					material = (ResourceMaterial*)App->resources->RequestResource((UID)payloadUID);
					usingCheckers = false;
					BindTexture(usingCheckers);
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::Spacing();
	}
}

void ComponentMaterial::BindTexture(bool checkers)
{
	if (checkers)
	{
		int checkersWidth = 64;
		int checkersHeight = 64;

		GLubyte checkerImage[64][64][3];

		for (int i = 0; i < checkersHeight; i++) {
			for (int j = 0; j < checkersWidth; j++) {
				int c = (((i & 0x4) == 0) ^ ((j & 0x4) == 0)) * 255;
				checkerImage[i][j][0] = (GLubyte)c;
				checkerImage[i][j][1] = (GLubyte)c;
				checkerImage[i][j][2] = (GLubyte)c;
			}
		}

		glBindTexture(GL_TEXTURE_2D, checkersId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, checkersWidth, checkersHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, checkerImage);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		if (material != nullptr) material->GenerateBuffers();
	}
}

void ComponentMaterial::OnLoad(const JSONReader& mat, Application* App)
{
	if (mat.HasMember("checkersId"))
	{
		checkersId = mat["checkersId"].GetInt();
	}

	if (mat.HasMember("usingCheckers"))
	{
		usingCheckers = mat["usingCheckers"].GetBool();
		BindTexture(usingCheckers);
	}

	material = nullptr;
	if (!usingCheckers)
	{
		if (mat.HasMember("Name"))
		{
			ResourceMaterial* rMat = (ResourceMaterial*)App->resources->RequestResource(mat["Name"].GetString());
			if (rMat != nullptr) material = rMat;
		}
		else if (mat.HasMember("Color"))
		{
			const rapidjson::Value& jDiffuse = mat["Diffuse"];
			float c[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			uint i = 0;
			for (rapidjson::Value::ConstValueIterator it = jDiffuse.Begin(); it != jDiffuse.End(); ++it, ++i)
			{
				c[i] = it->GetDouble();
			}
			Color color = Color(c[0], c[1], c[2], c[3]);
			ResourceMaterial* rMat = (ResourceMaterial*)App->resources->RequestResource(color);
			if (rMat != nullptr)
			{
				material = rMat;
			}
		}
		else if (mat.HasMember("UID"))
		{
			ResourceMaterial* rMat = (ResourceMaterial*)App->resources->RequestResource(mat["UID"].GetInt());
			if (rMat != nullptr) material = rMat;
		}
	}
}

void ComponentMaterial::OnSave(JSONWriter& writer) const
{
	writer.String("Material");
	writer.StartObject();
	if (!usingCheckers)
	{
		writer.String("UID"); writer.Int(material->GetUID());
		if (material->name != "Color_texture")
		{
			writer.String("Name"); writer.String(material->GetAssetFile());
		}
		else
		{
			writer.String("Color"); writer.Bool(material->usingColor);
			writer.String("Diffuse");
			writer.StartArray();
			writer.Double(material->diffuse.x);
			writer.Double(material->diffuse.y);
			writer.Double(material->diffuse.z);
			writer.Double(material->diffuse.w);
			writer.EndArray();
		}
	}
	writer.String("usingCheckers"); writer.Bool(usingCheckers);
	writer.String("checkersId"); writer.Int(checkersId);
	writer.EndObject();
}