#include "RenderGlobals.h"
#include "Gameobject.h"

Material::Material(bool active) : Component(type, active)
{
	type = ComponentTypes::MATERIAL;

	name = "Default_texture";
	path = ASSETS_FOLDER;
	path.append(TEXTURES_FOLDER);
	id = -1;
	textureBuf = -1;
	format = -1;
	formatUnsigned = -1;
	data = nullptr;
	width = -1;
	height = -1;
	checkers = false;
	usingColor = false;
	diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
}

Material::Material(std::string n, std::string p, uint texId, uint texBuf, int f, uint fUnsigned, GLubyte* d, int w, int h, bool active) : Component(type, active)
{
	type = ComponentTypes::MATERIAL;

	name = n;
	path = p;
	id = texId;
	textureBuf = texBuf;
	format = f;
	formatUnsigned = fUnsigned;
	data = d;
	width = w;
	height = h;
	checkers = false;
	usingColor = false;
	diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
}

Material::~Material()
{
	name.clear();
	path.clear();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteBuffers(1, &textureBuf);
	glDeleteTextures(1, &id);

	data = nullptr;
}

void Material::DrawInspector()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Material", &active);
		ImGui::SameLine();
		if (ImGui::Checkbox("Checkers", &checkers)) BindTexture(data);

		ImGui::Separator();

		if (data != nullptr)
		{
			IMGUI_PRINT(IMGUI_YELLOW, "Diffuse Texture:", "%s", name.c_str());
			IMGUI_PRINT(IMGUI_YELLOW, "Path:", "%s", path.c_str());
			IMGUI_PRINT(IMGUI_YELLOW, "Width:", "%d", width); ImGui::SameLine(); IMGUI_PRINT(IMGUI_YELLOW, "Height:", "%d", height);
			ImGui::Text("Image:");
			ImGui::Image((ImTextureID)id, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
		}
		ImGui::Spacing();
	}
}

bool Material::SetTexture(Material* texture)
{
	if (texture != nullptr)
	{
		usingColor = texture->usingColor;
		if (usingColor)
		{
			SetTexture(texture->diffuse);
			return true;
		}
		else if(texture->data != nullptr)
		{
			name = texture->name;
			path = texture->path;
			id = texture->id;
			format = texture->format;
			formatUnsigned = texture->formatUnsigned;
			width = texture->width;
			height = texture->height;

			BindTexture(texture->data);

			usingColor = texture->usingColor;
			diffuse = texture->diffuse;
			data = texture->data;
			return true;
		}
	}
	SetDefaultTexture();
	return false;
}

bool Material::SetTexture(Color c)
{
	name = "Color_texture";
	path = ASSETS_FOLDER;
	path.append(TEXTURES_FOLDER);
	format = GL_RGBA;
	formatUnsigned = GL_RGBA;
	width = 128;
	height = 128;
	diffuse = c;
	data = BindTexture();
	return true;
}

void Material::SetDefaultTexture()
{
	name = "Default_texture";
	path = ASSETS_FOLDER;
	path.append(TEXTURES_FOLDER);
	format = GL_RGBA;
	formatUnsigned = GL_RGBA;
	width = 128;
	height = 128;
	data = BindTexture();
}

GLubyte* Material::BindTexture(GLubyte* texData)
{
	if (checkers)
	{
		GLubyte checkerTex[128][128][4];

		for (int i = 0; i < 128; ++i)
		{
			for (int j = 0; j < 128; ++j)
			{
				int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
				checkerTex[i][j][0] = (GLubyte)c;
				checkerTex[i][j][1] = (GLubyte)c;
				checkerTex[i][j][2] = (GLubyte)c;
				checkerTex[i][j][3] = (GLubyte)255;
			}
		}
		texData = (GLubyte*)checkerTex;
	}
	else if (usingColor)
	{
		GLubyte colorTex[128][128][4];

		for (int i = 0; i < 128; ++i)
		{
			for (int j = 0; j < 128; ++j)
			{
				colorTex[i][j][0] = (GLubyte)(diffuse.r * 255);
				colorTex[i][j][1] = (GLubyte)(diffuse.g * 255);
				colorTex[i][j][2] = (GLubyte)(diffuse.b * 255);
				colorTex[i][j][3] = (GLubyte)(diffuse.a * 255);
			}
		}
		texData = (GLubyte*)colorTex;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (id == -1)
	{
		glGenTextures(1, &id);
	}
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (checkers)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, formatUnsigned, GL_UNSIGNED_BYTE, texData);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return texData;
}