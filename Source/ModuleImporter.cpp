#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleSceneIntro.h"
#include "il.h"
#include "ilu.h"
#include "ilut.h"

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

	// DevIL
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return ret;
}

bool ModuleImporter::Start()
{
	//ImportScene("Assets/warrior.fbx");
	ImportScene("Assets/Models/BakerHouse.fbx");
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

Texture* ModuleImporter::LoadTexture(const char* path)
{
	uint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);

	std::string name;
	SplitPath(path, nullptr, &name);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	Texture* texture = new Texture();

	if(id ==0)
		if (App->gui != nullptr) App->gui->LogConsole(LOG("Error generation the image buffer: %s, %d",path,ilGetError()));

	// TODO:: Change to ilLoadL
	if (ilLoadImage(path))
	{
		if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			ILenum error = ilGetError();

			if (error != IL_NO_ERROR)
			{
				if (App->gui != nullptr) App->gui->LogConsole(LOG("Texture error: %d, %s", error, iluErrorString(error)));
			}
			else
			{
				if (App->gui != nullptr) App->gui->LogConsole(LOG("Texture loaded successfully from: %s", path));

				texture->id = id;
				texture->name = name;
				texture->data = ilGetData();
				texture->width = ilGetInteger(IL_IMAGE_WIDTH);
				texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
				texture->format = texture->formatUnsigned = ilGetInteger(IL_IMAGE_FORMAT);
				texture->path = path;
			}
		}
		else
		{
			if (App->gui != nullptr) App->gui->LogConsole(LOG("Error converting texture: %d, %s", ilGetError(), iluErrorString(ilGetError())));
		}
	}
	else
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("Error loading the texture from %s: %d, %s", path, ilGetError(), iluErrorString(ilGetError())));
	}

	return texture;
}

Texture* ModuleImporter::LoadTexture(const aiScene* scene, aiMesh* mesh, const char* path)
{
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	aiString texPath;
	aiGetMaterialTexture(material, aiTextureType::aiTextureType_DIFFUSE, mesh->mMaterialIndex, &texPath);

	Texture* texture = new Texture();

	if (texPath.length != 0)
	{
		std::string newPath = UnifyPath(ASSETS_FOLDER, TEXTURES_FOLDER, texPath.data);
		texture = LoadTexture(newPath.c_str());
		return texture;
	}
	return nullptr;
}

void ModuleImporter::ImportScene(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes; i++)
		{
			Mesh* m = ImportModel(scene->mMeshes[i]);
			m->SetTexture(LoadTexture(scene, scene->mMeshes[i], path));
			m->GenerateBuffers();
		}
		aiReleaseImport(scene);
	}
	else
	{
		if (App->gui != nullptr) App->gui->LogConsole(LOG("Error loading scene with path %s", path));
	}
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

		listMesh.push_back(m);
		return m;
	}

	return nullptr;
}

void ModuleImporter::SplitPath(const char* fullPath, std::string* path, std::string* fileName)
{
	if (fullPath != nullptr)
	{
		std::string full(fullPath);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				*path = full.substr(0, pos_separator + 1).c_str();
			else
				path->clear();
		}

		if (fileName != nullptr)
		{
			if (pos_separator < full.length())
				*fileName = full.substr(pos_separator + 1, std::string::npos);
		}
	}
}

std::string ModuleImporter::UnifyPath(const char* path, const char* subDir, const char* name)
{
	std::string newPath;
	newPath.assign(path);
	newPath.append(subDir);
	newPath.append(name);
	return newPath;
}