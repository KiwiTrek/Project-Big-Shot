#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Globals.h"
#include <string>

typedef unsigned char GLubyte;
class GameObject;

class Material : public Component
{
public:
	Material(bool active = true);
	Material(std::string n, std::string p, uint texId, uint texBuf, int f, uint fUnsigned, GLubyte* data, int w, int h, bool active = true);
	~Material();

	bool SetTexture(Material* texture);
	bool SetTexture(Color c);
	void DrawInspector();

private:
	void SetDefaultTexture();
	GLubyte* BindTexture(GLubyte* texData = nullptr);
	//void BindTexture(Color c);
	//GLubyte* CheckersTexture();
	//GLubyte* ColorTexture(Color c);

public:
	std::string name;
	std::string path;

	uint id = -1;
	uint textureBuf = -1;

	int format = -1;
	uint formatUnsigned = -1;
	GLubyte* data = nullptr;
	int width = -1;
	int height = -1;

	bool checkers = false;
	bool usingColor = false;

	Color diffuse;
};

#endif //!__COMPONENT_MATERIAL_H__