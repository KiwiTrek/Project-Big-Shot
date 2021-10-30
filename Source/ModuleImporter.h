#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include "SDL.h"
#include "Gameobject.h"

class Application;
struct aiMesh;
struct aiScene;
struct aiNode;

class ModuleImporter : public Module
{
public:

	ModuleImporter(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleImporter();

	bool Init();
	bool Start();
	bool CleanUp();

	std::string GetFileName(const char* path);
	void ImportScene(const char* path, const char* rootName);
	Material* LoadTexture(const char* path);
	Material* LoadTexture(const aiScene* scene, aiNode* n, const char* path);
	Transform* LoadTransform(aiNode* n);

private:
	Mesh* ImportModel(const aiScene* scene, aiNode* node, const char* path);
	GameObject* ImportChild(const aiScene* scene, aiNode* n, aiNode* parentN, GameObject* parentGameObject, const char* path, const char* rootName = nullptr);
	void SplitPath(const char* fullPath, std::string* path, std::string* fileName);
	std::string UnifyPath(const char* path, const char* subDir, const char* name);
};

#endif // !__MODULE_FILE_SYSTEM_H__