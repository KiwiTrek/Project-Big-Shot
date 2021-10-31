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
    Material(std::string n, std::string p, uint texId,uint texBuf, int f, uint fUnsigned, GLubyte* data, int w, int h, bool active);
    ~Material();
    bool SetTexture(Material* texture);
    void DrawInspector();

private:
    void SetDefaultTexture();
    void BindTexture(GLubyte* texData);
    GLubyte* CheckersTexture();

public:
    std::string name = "defaultTex";
    std::string path = "Assets/Textures/";

    uint id = -1;
    uint textureBuf = -1;

    int format = -1;
    uint formatUnsigned = -1;
    GLubyte* data = nullptr;
    int width = -1;
    int height = -1;

    bool checkers = false;
};

#endif //!__COMPONENT_MATERIAL_H__