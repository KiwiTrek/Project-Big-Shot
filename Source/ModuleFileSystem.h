#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include "SDL.h"

class Application;

struct MeshData
{
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* indices = nullptr;
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertices = nullptr;
};

class ModuleFileSystem : public Module
{
public:

	ModuleFileSystem(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleFileSystem();

	bool Init();
	bool CleanUp();

	void ImportModel(const char* path);

public:
	std::vector<MeshData*> meshList;

private:

};

#endif // !__MODULE_FILE_SYSTEM_H__