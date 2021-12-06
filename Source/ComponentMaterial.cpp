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
				IMGUI_PRINT(IMGUI_YELLOW, "UID: ", "%d", material->GetUID());
				IMGUI_PRINT(IMGUI_YELLOW, "Diffuse Texture:", "%s", material->name.c_str());
				IMGUI_PRINT(IMGUI_YELLOW, "Path:", "%s", material->path.c_str());
				IMGUI_PRINT(IMGUI_YELLOW, "Width:", "%d", material->width); ImGui::SameLine(); IMGUI_PRINT(IMGUI_YELLOW, "Height:", "%d", material->height);
				ImGui::Text("Image:");
				ImGui::Image((ImTextureID)material->texId, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
			}
		}
		else
		{
			IMGUI_PRINT(IMGUI_YELLOW, "Diffuse Texture: ", "- none -");
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
				int c = ((((i & 0x4) == 0) ^ (((j & 0x4)) == 0))) * 255;
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
	if (mat.HasMember("UID"))
	{
		ResourceMaterial* rMat = (ResourceMaterial*)App->resources->RequestResource(mat["UID"].GetInt());
		if (rMat == nullptr)
		{
			LOG("NO FUNCIONA");
		}
		else
		{
			material = rMat;
		}
	}

	if (mat.HasMember("usingCheckers"))
	{
		usingCheckers = mat["usingCheckers"].GetBool();
	}

	if (mat.HasMember("checkersId"))
	{
		checkersId = mat["checkersId"].GetInt();
	}
}

void ComponentMaterial::OnSave(JSONWriter& writer) const
{
	writer.String("Material");
	writer.StartObject();
	if (!usingCheckers)
	{
		writer.String("UID");
		writer.Int(material->GetUID());
	}
	writer.String("usingCheckers");
	writer.Bool(usingCheckers);
	writer.String("checkersId");
	writer.Int(checkersId);
	writer.EndObject();
}