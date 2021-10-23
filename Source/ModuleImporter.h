#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include "SDL.h"
#include "Mesh.h"

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
	Mesh* ImportModel(aiMesh* mesh);
	Texture* LoadTexture(const char* path);
	Texture* LoadTexture(const aiScene* scene, aiMesh* mesh, const char* path);
	void AddPrimitive(Mesh* p);
	void SplitPath(const char* fullPath, std::string* path, std::string* fileName);
	std::string UnifyPath(const char* path, const char* subDir, const char* name);

public:

	std::vector<Mesh*> listMesh;
};

#endif // !__MODULE_FILE_SYSTEM_H__