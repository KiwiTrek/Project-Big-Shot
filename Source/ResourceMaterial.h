#ifndef __RESOURCE_MATERIAL_H__
#define __RESOURCE_MATERIAL_H__

#include "Resource.h"
#include "Color.h"

typedef unsigned char GLubyte;

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(UID uid);
	ResourceMaterial(UID uid, Color c);
	~ResourceMaterial();

	void GenerateBuffers();

public:
	std::string name;
	std::string path;

	uint texId = -1;

	int format = -1;
	uint formatUnsigned = -1;
	GLubyte* data = nullptr;
	int width = -1;
	int height = -1;

	Color diffuse;
};

#endif // !__RESOURCE_MATERIAL_H__