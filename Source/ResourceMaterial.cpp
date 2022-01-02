#include "RenderGlobals.h"
#include "ResourceMaterial.h"

ResourceMaterial::ResourceMaterial(UID uid) : Resource(uid, Resource::Type::MATERIAL)
{
	name = "Default_texture";
	texId = -1;
	format = -1;
	formatUnsigned = -1;
	data = nullptr;
	usingColor = false;
	width = -1;
	height = -1;
	diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
}

ResourceMaterial::ResourceMaterial(UID uid, Color c) : Resource(uid, Resource::Type::MATERIAL)
{
	data = new GLubyte[4];
	data[0] = (GLubyte)(c.x * 255);
	data[1] = (GLubyte)(c.y * 255);
	data[2] = (GLubyte)(c.z * 255);
	data[3] = (GLubyte)(c.w * 255);

	name = "Color_texture";
	texId = -1;
	format = GL_RGBA;
	formatUnsigned = GL_RGBA;
	width = 1;
	height = 1;
	diffuse = c;
	usingColor = true;
}

ResourceMaterial::~ResourceMaterial()
{
	name.clear();
	path.clear();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &texId);

	data = nullptr;
}

void ResourceMaterial::GenerateBuffers()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	if (texId == -1) glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, formatUnsigned, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
}