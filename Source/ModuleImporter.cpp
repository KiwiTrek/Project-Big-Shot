#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "ModuleGameObjects.h"

#include "il.h"
#include "ilu.h"
#include "ilut.h"

#include "cimport.h"
#include "scene.h"
#include "postprocess.h"
#include "version.h"

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
	LOG_CONSOLE("Init File System");
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	// DevIL
	LOG_CONSOLE("Init DevIL");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return ret;
}

bool ModuleImporter::Start()
{
	//ImportScene("Assets/warrior.fbx");
	ImportScene("Assets/Models/BakerHouse.fbx", "BakerHouse");
	return true;
}

// Called before quitting
bool ModuleImporter::CleanUp()
{
	LOG("Destroying File System");

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

Material* ModuleImporter::LoadTexture(const char* path)
{
	uint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);

	std::string name = GetFileName(path);

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	Material* texture = new Material();

	if (id == 0)
		LOG_CONSOLE("Error generation the image buffer: %s, %d", path, ilGetError());

	// TODO:: Change to ilLoadL
	if (ilLoadImage(path))
	{
		if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			ILenum error = ilGetError();

			if (error != IL_NO_ERROR)
			{
				LOG_CONSOLE("Texture error: %d, %s", error, iluErrorString(error));
			}
			else
			{
				LOG_CONSOLE("Texture loaded successfully from: %s", path);

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
			LOG_CONSOLE("Error converting texture: %d, %s", ilGetError(), iluErrorString(ilGetError()));
		}
	}
	else
	{
		LOG_CONSOLE("Error loading the texture from %s: %d, %s", path, ilGetError(), iluErrorString(ilGetError()));
	}

	return texture;
}

void ModuleImporter::ImportScene(const char* path, const char* rootName)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		GameObject* root = new GameObject("Empty");
		aiNode* rootNode = scene->mRootNode;
		root = ImportChild(scene, rootNode, nullptr, nullptr, path, rootName);
		App->gameObjects->AddGameobject(root);
		LOG_CONSOLE("Successfully loaded scene with path %s", path);
		aiReleaseImport(scene);
	}
	else
	{
		LOG_CONSOLE("Error loading scene with path %s", path);
	}
}

GameObject* ModuleImporter::ImportChild(const aiScene* scene, aiNode* n, aiNode* parentN, GameObject* parentGameObject, const char* path, const char* rootName)
{
	GameObject* g = nullptr;

	if (rootName != nullptr)
	{
		g = new GameObject(rootName);
	}
	else
	{
		g = new GameObject(n->mName.C_Str());
	}

	if (parentGameObject != nullptr)
	{
		parentGameObject->AddChild(g);
		g->SetParent(parentGameObject);
	}

	Transform* t = (Transform*)g->CreateComponent(ComponentTypes::TRANSFORM, MeshTypes::NONE, LoadTransform(n));
	if (t->GetScale().x >= 100.0f)
	{
		t->SetScale(1.0f, 1.0f, 1.0f);
		t->UpdateLocalTransform();
	}

	if (n->mMeshes != nullptr)
	{
		g->name = n->mName.C_Str();

		Material* mat = (Material*)g->CreateComponent(ComponentTypes::MATERIAL);
		mat->SetTexture(LoadTexture(scene, n, path));

		Mesh* mesh = ImportModel(scene, n, path);
		g->CreateComponent(mesh);
		mesh->GenerateBuffers();
	}

	for (size_t i = 0; i < n->mNumChildren; i++)
	{
		ImportChild(scene, n->mChildren[i], n, g, path);
	}

	return g;
}

Material* ModuleImporter::LoadTexture(const aiScene* scene, aiNode* n, const char* path)
{
	aiMesh* aiMesh = scene->mMeshes[*n->mMeshes];

	aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
	aiString texPath;
	aiGetMaterialTexture(material, aiTextureType::aiTextureType_DIFFUSE, aiMesh->mMaterialIndex, &texPath);

	Material* texture = new Material();

	if (texPath.length != 0)
	{
		std::string newPath = UnifyPath(ASSETS_FOLDER, TEXTURES_FOLDER, texPath.data);
		texture = LoadTexture(newPath.c_str());
		return texture;
	}
	return nullptr;
}

Mesh* ModuleImporter::ImportModel(const aiScene* scene, aiNode* node, const char* path)
{
	Mesh* m = new Mesh();
	aiMesh* aiMesh = scene->mMeshes[*node->mMeshes];
	m->vertexNum = aiMesh->mNumVertices;
	m->vertices = new float[m->vertexNum * 3];
	memcpy(m->vertices, aiMesh->mVertices, sizeof(float) * m->vertexNum * 3);

	LOG_CONSOLE("Loaded new mesh with %d vertices", m->vertexNum);

	if (aiMesh->HasFaces())
	{
		m->indexNum = aiMesh->mNumFaces * 3;
		m->indices = new uint[m->indexNum];
		for (uint j = 0; j < aiMesh->mNumFaces; j++)
		{
			if (aiMesh->mFaces[j].mNumIndices != 3)
			{
				// Quad
				LOG_CONSOLE("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&m->indices[j * 3], aiMesh->mFaces[j].mIndices, sizeof(uint) * 3);
			}
		}

		m->texCoords = new float[m->vertexNum * 2]();
		m->colors = new float[m->indexNum * 4]();
		m->normals = new float[aiMesh->mNumVertices * 3]();

		int t = 0;

		for (uint v = 0, n = 0, tx = 0, c = 0; v < aiMesh->mNumVertices; v++, n += 3, c += 4, tx += 2)
		{
			if (aiMesh->HasNormals())
			{
				m->normals[n] = aiMesh->mNormals[v].x;
				m->normals[n + 1] = aiMesh->mNormals[v].y;
				m->normals[n + 2] = aiMesh->mNormals[v].z;
			}

			if (aiMesh->HasVertexColors(v))
			{
				m->colors[c] = aiMesh->mColors[v]->r;
				m->colors[c + 1] = aiMesh->mColors[v]->g;
				m->colors[c + 2] = aiMesh->mColors[v]->b;
				m->colors[c + 3] = aiMesh->mColors[v]->a;
			}
			else
			{
				m->colors[c] = 0.0f;
				m->colors[c + 1] = 0.0f;
				m->colors[c + 2] = 0.0f;
				m->colors[c + 3] = 0.0f;
			}

			if (aiMesh->mTextureCoords[0])
			{
				m->texCoords[tx] = aiMesh->mTextureCoords[0][v].x;
				m->texCoords[tx + 1] = aiMesh->mTextureCoords[0][v].y;
			}
			else
			{
				m->texCoords[tx] = 0.0f;
				m->texCoords[tx + 1] = 0.0f;
			}
			t = tx;
		}

		return m;
	}

	return nullptr;
}

Transform* ModuleImporter::LoadTransform(aiNode* n)
{
	aiVector3D p, s;
	aiQuaternion r;

	n->mTransformation.Decompose(s, r, p);

	float3 pos(p.x, p.y, p.z);
	float3 scale(s.x, s.y, s.z);
	Quat rot(r.x, r.y, r.z, r.w);

	Transform* t = new Transform();
	t->SetPos(pos);
	t->SetRot(rot);
	t->SetScale(scale);
	t->UpdateLocalTransform();

	return t;
}

void ModuleImporter::GetAssimpVersion(int& major, int& minor, int& patch)
{
	major = aiGetVersionMajor();
	minor = aiGetVersionMinor();
	patch = aiGetVersionRevision();
}

std::string ModuleImporter::GetFileName(const char* path)
{
	std::string name;
	SplitPath(path, nullptr, &name);
	return name;
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