#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include "SDL.h"
#include "Mesh.h"

class Application;
struct aiMesh;

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleFileSystem();

	bool Init();
	bool CleanUp();

	uint ImportScene(const char* path);
	CustomMesh* ImportModel(aiMesh* mesh);
	void AddPrimitive(Mesh* p);

public:

	std::vector<Mesh*> listMesh;

private:

};

#endif // !__MODULE_FILE_SYSTEM_H__