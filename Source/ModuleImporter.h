#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include "SDL.h"
#include "Gameobject.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"

class Application;
struct aiMesh;
struct aiScene;
struct aiNode;
class ResourceMaterial;

class ModuleImporter : public Module
{
public:
	ModuleImporter(Application* app, bool startEnabled = true);
	~ModuleImporter();

	bool Init();
	bool Start();
	bool CleanUp();

	void GetAssimpVersion(int& major, int& minor, int& patch);
	void ImportScene(const char* path, const char* rootName);
	ResourceMaterial* LoadTexture(const aiScene* scene, aiNode* n);
	ResourceMaterial* LoadTexture(const char* path);
	ComponentTransform* LoadTransform(aiNode* n);

private:
	ResourceMesh* ImportModel(const aiScene* scene, aiNode* node);
	GameObject* ImportChild(const aiScene* scene, aiNode* n, aiNode* parentN, GameObject* parentGameObject, const char* rootName = nullptr);
};

#endif // !__MODULE_FILE_SYSTEM_H__