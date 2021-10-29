#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include "SDL.h"
#include "Gameobject.h"

class Application;
struct aiMesh;
struct aiScene;

class ModuleImporter : public Module
{
public:

	ModuleImporter(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleImporter();

	bool Init();
	bool Start();
	bool CleanUp();

	void ImportScene(const char* path);
	Material* LoadTexture(const char* path);
	Material* LoadTexture(const aiScene* scene, aiMesh* mesh, const char* path);
	void AddGameobject(Gameobject* g);

private:
	Mesh* ImportModel(aiMesh* mesh);
	void SplitPath(const char* fullPath, std::string* path, std::string* fileName);
	std::string UnifyPath(const char* path, const char* subDir, const char* name);

public:

	std::vector<Gameobject*> gameobjectList;
};

#endif // !__MODULE_FILE_SYSTEM_H__