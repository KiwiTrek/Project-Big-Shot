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

	return ret;
}

bool ModuleImporter::Start()
{
	//ImportScene("Assets/warrior.fbx");
	//ImportScene("Assets/BakerHouse.fbx");
	return true;
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
			ImportModel(scene->mMeshes[i]);
		}
		aiReleaseImport(scene);
	}
	else
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("Error loading scene with path %s", path));
	}

	return scene->mNumMeshes;
}

Mesh* ModuleImporter::ImportModel(aiMesh* mesh)
{
	Mesh* m = new Mesh();
	m->vertexNum = mesh->mNumVertices;
	m->vertices = new float[m->vertexNum * 3];
	memcpy(m->vertices, mesh->mVertices, sizeof(float) * m->vertexNum * 3);

	if (App->gui != nullptr) App->gui->LogConsole(LOG("Loaded new mesh with %d vertices", m->vertexNum));

	if (mesh->HasFaces())
	{
		m->indexNum = mesh->mNumFaces * 3;
		m->indices = new uint[m->indexNum];
		for (uint j = 0; j < mesh->mNumFaces; j++)
		{
			if (mesh->mFaces[j].mNumIndices != 3)
			{
				// Quad
				if (App->gui != nullptr) App->gui->LogConsole(LOG("WARNING, geometry face with != 3 indices!"));
			}
			else
			{
				memcpy(&m->indices[j * 3], mesh->mFaces[j].mIndices, sizeof(uint) * 3);
			}
		}

		m->texCoords = new float[m->vertexNum * 2]();
		m->colors = new float[m->indexNum * 4]();	//RGBA
		m->normals = new float[mesh->mNumVertices * 3]();

		int t = 0;

		for (uint v = 0, n = 0, tx = 0, c = 0; v < mesh->mNumVertices; v++, n += 3, c += 4, tx += 2)
		{
			if (mesh->HasNormals())
			{
				m->normals[n] = mesh->mNormals[v].x;
				m->normals[n + 1] = mesh->mNormals[v].y;
				m->normals[n + 2] = mesh->mNormals[v].z;
			}

			if (mesh->HasVertexColors(v))
			{
				m->colors[c] = mesh->mColors[v]->r;
				m->colors[c + 1] = mesh->mColors[v]->g;
				m->colors[c + 2] = mesh->mColors[v]->b;
				m->colors[c + 3] = mesh->mColors[v]->a;
			}
			else
			{
				m->colors[c] = 0.0f;
				m->colors[c + 1] = 0.0f;
				m->colors[c + 2] = 0.0f;
				m->colors[c + 3] = 0.0f;
			}

			if (mesh->mTextureCoords[0])
			{
				m->texCoords[tx] = mesh->mTextureCoords[0][v].x;
				m->texCoords[tx + 1] = mesh->mTextureCoords[0][v].y;
			}
			else
			{
				m->texCoords[tx] = 0.0f;
				m->texCoords[tx + 1] = 0.0f;
			}
			t = tx;
		}

		m->GenerateBuffers();

		listMesh.push_back(m);
		return m;
	}

	return nullptr;
}