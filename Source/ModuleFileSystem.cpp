#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "cimport.h"
#include "scene.h"
#include "postprocess.h"

ModuleFileSystem::ModuleFileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "file_system";
}

// Destructor
ModuleFileSystem::~ModuleFileSystem()
{
}

// Called before render is available
bool ModuleFileSystem::Init()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Init File System"));
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

// Called before quitting
bool ModuleFileSystem::CleanUp()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Destroying File System"));

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void ModuleFileSystem::ImportModel(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			MeshData* tmp = new MeshData;
			tmp->num_vertex = scene->mMeshes[i]->mNumVertices;
			tmp->vertices = new float[tmp->num_vertex * 3];
			memcpy(tmp->vertices, scene->mMeshes[i]->mVertices, sizeof(float) * tmp->num_vertex * 3);
			if (App->gui != nullptr) App->gui->LogConsole(LOG("Loaded vertex array with %d vertices", tmp->num_vertex));

			tmp->num_index = scene->mMeshes[i]->mNumFaces * 3;
			tmp->indices = new uint[tmp->num_index];
			for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; i++)
			{
				if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
				{
					// Quad
					if(App->gui != nullptr) App->gui->LogConsole(LOG("WARNING, geometry face with != 3 indices!"));
				}
				else
				{
					memcpy(&tmp->indices[i * 3], scene->mMeshes[i]->mFaces[j].mIndices, sizeof(uint) * 3);
				}
			}

			if (App->gui != nullptr) App->gui->LogConsole(LOG("Loaded vertex array with %d vertices", tmp->num_vertex));

			meshList.push_back(tmp);
		}
		aiReleaseImport(scene);
	}
	else
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("Error loading scene with path %s", path));
	}
}