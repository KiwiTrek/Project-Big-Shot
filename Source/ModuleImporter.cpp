#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleSceneIntro.h"

#include "cimport.h"
#include "scene.h"
#include "postprocess.h"

ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "file_system";
}

// Destructor
ModuleImporter::~ModuleImporter()
{
}

// Called before render is available
bool ModuleImporter::Init()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Init File System"));
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	//ImportScene("Assets/warrior.fbx");
	ImportScene("Assets/BakerHouse.fbx");

	return ret;
}

// Called before quitting
bool ModuleImporter::CleanUp()
{
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Destroying File System"));

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void ModuleImporter::AddPrimitive(Mesh* p)
{
	listMesh.push_back(p);
}

uint ModuleImporter::ImportScene(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			App->sceneIntro->customMeshes.push_back(ImportModel(scene->mMeshes[i]));
		}
		aiReleaseImport(scene);
	}
	else
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("Error loading scene with path %s", path));
	}

	return scene->mNumMeshes;
}

CustomMesh* ModuleImporter::ImportModel(aiMesh* mesh)
{
	MeshData* tmp = new MeshData;
	tmp->num_vertex = mesh->mNumVertices;
	tmp->vertices = new float[tmp->num_vertex * 3];
	memcpy(tmp->vertices, mesh->mVertices, sizeof(float) * tmp->num_vertex * 3);
	if (App->gui != nullptr) App->gui->LogConsole(LOG("Loaded new mesh with %d vertices", tmp->num_vertex));

	if (mesh->HasFaces())
	{
		//if (mesh->HasNormals())
		//{
		//	tmp->num_normals = mesh->mNumVertices;
		//	for (int k = 0; k < tmp->num_normals; k++)
		//	{
		//		tmp->normals[k].Set((float)mesh->mNormals[k].x, (float)mesh->mNormals[k].y, (float)mesh->mNormals[k].z);
		//	}
		//}

		tmp->num_index = mesh->mNumFaces * 3;
		tmp->indices = new uint[tmp->num_index];
		for (uint j = 0; j < mesh->mNumFaces; j++)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				// Quad
				if (App->gui != nullptr) App->gui->LogConsole(LOG("WARNING, geometry face with != 3 indices!"));
			}
			else
			{
				memcpy(&tmp->indices[j * 3], mesh->mFaces[j].mIndices, sizeof(uint) * 3);
			}
		}

		CustomMesh* m = new CustomMesh(tmp);
		listMesh.push_back(m);
		return m;
	}

	return nullptr;
}