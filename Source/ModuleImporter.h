#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include "SDL.h"
#include "Mesh.h"

class Application;
struct aiMesh;

class ModuleImporter : public Module
{
public:

	ModuleImporter(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleImporter();

	bool Init();
	bool Start();
	bool CleanUp();

	uint ImportScene(const char* path);
	Mesh* ImportModel(aiMesh* mesh);
	void AddPrimitive(Mesh* p);

public:

	std::vector<Mesh*> listMesh;

private:

};

#endif // !__MODULE_FILE_SYSTEM_H__