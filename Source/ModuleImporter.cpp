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

ModuleImporter::ModuleImporter(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "file_system";
}

ModuleImporter::~ModuleImporter()
{}

bool ModuleImporter::Init()
{
	LOG_CONSOLE("Init File System");
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	LOG_CONSOLE("Init DevIL");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return ret;
}

bool ModuleImporter::Start()
{
	//ImportScene("Assets/Resources/Models/Baker_house.fbx", "Baker_house");
	ImportScene("Assets/Resources/Models/Street_environment.fbx", "Street_environment");
	return true;
}

bool ModuleImporter::CleanUp()
{
	LOG("Destroying File System");

	// Detach log stream
	aiDetachAllLogStreams();

	return true;
}

Material* ModuleImporter::LoadTexture(const char* path)
{
	uint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);

	std::string name = GetFileName(path);
	std::string newPath = UnifyPath(ASSETS_FOLDER, TEXTURES_FOLDER, name.c_str());

	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	Material* texture = new Material();

	if (id == 0)
	{
		LOG_CONSOLE("Error generation the image buffer: %s, %d", path, ilGetError());
	}

	// TODO:: Change to ilLoadL
	if (ilLoadImage(newPath.c_str()))
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
				LOG_CONSOLE("Texture loaded successfully from: %s", newPath.c_str());

				texture->id = id;
				texture->name = name;
				texture->data = ilGetData();
				texture->width = ilGetInteger(IL_IMAGE_WIDTH);
				texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
				texture->format = texture->formatUnsigned = ilGetInteger(IL_IMAGE_FORMAT);
				texture->path = newPath.c_str();
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

		root = ImportChild(scene, rootNode, nullptr, nullptr, rootName);
		App->gameObjects->AddGameobject(root);

		LOG_CONSOLE("Successfully loaded scene with path %s", path);
		aiReleaseImport(scene);
	}
	else
	{
		LOG_CONSOLE("Error loading scene with path %s", path);
	}
}

GameObject* ModuleImporter::ImportChild(const aiScene* scene, aiNode* n, aiNode* parentN, GameObject* parentGameObject, const char* rootName)
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
		g->parent = parentGameObject;
	}

	Transform* t = (Transform*)g->CreateComponent(ComponentTypes::TRANSFORM, MeshTypes::NONE, LoadTransform(n));
	if (t->GetScale().x >= 100.0f)
	{
		t->SetScale(1.0f, 1.0f, 1.0f);
	}
	t->UpdateGlobalTransform();
	if (g->parent != nullptr) g->parent->UpdateChildrenTransforms();

	if (n->mMeshes != nullptr)
	{
		g->name = n->mName.C_Str();

		Material* mat = (Material*)g->CreateComponent(ComponentTypes::MATERIAL);
		mat->SetTexture(LoadTexture(scene, n));

		Mesh* mesh = ImportModel(scene, n);
		g->CreateComponent(mesh);
		mesh->GenerateBuffers();
	}

	for (size_t i = 0; i < n->mNumChildren; i++)
	{
		ImportChild(scene, n->mChildren[i], n, g);
	}

	return g;
}

Material* ModuleImporter::LoadTexture(const aiScene* scene, aiNode* n)
{
	aiMaterial* material = nullptr;
	aiString texPath;
	aiMesh* aiMesh = scene->mMeshes[*n->mMeshes];
	if (aiMesh->mMaterialIndex >= 0)
	{
		material = scene->mMaterials[aiMesh->mMaterialIndex];
		material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texPath);
		/* 0 -> All textures only have 1 diffuse map.
		* If there are multiple diffuse maps, it'll only render the first one
		* As far as we know, this shouldn't happen.
		*/
	}

	Material* texture = new Material();

	if (texPath.length != 0)
	{
		std::string newPath = UnifyPath(ASSETS_FOLDER, TEXTURES_FOLDER, texPath.data);
		texture = LoadTexture(newPath.c_str());
		return texture;
	}
	else
	{
		LOG_CONSOLE("No texture found inside model, looking for a Color...");

		aiColor4D diffuse;
		if (material != nullptr && aiReturn::AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		{
			uint id = 0;
			ilGenImages(1, &id);
			ilBindImage(id);
			texture->id = id;
			texture->usingColor = true;
			texture->diffuse = Color(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
			//----------
			//uint id = 0;
			//ilGenImages(1, &id);
			//texture->id = id;
			//texture->SetTexture(Color(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
			return texture;
		}
		else
		{
			LOG_CONSOLE("No Color found!");
		}
	}


	return nullptr;
}

Mesh* ModuleImporter::ImportModel(const aiScene* scene, aiNode* node)
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
				LOG_CONSOLE("ERROR: Geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&m->indices[j * 3], aiMesh->mFaces[j].mIndices, sizeof(uint) * 3);
			}
		}

		m->texCoords = new float[m->vertexNum * 2]();
		m->colors = new float[m->indexNum * 4]();
		m->normals = new float[aiMesh->mNumVertices * 3]();

		uint n = 0, tx = 0, c = 0;
		for (uint v = 0; v < aiMesh->mNumVertices; v++)
		{
			if (aiMesh->HasNormals())
			{
				m->normals[n] = aiMesh->mNormals[v].x;
				m->normals[n + 1] = aiMesh->mNormals[v].y;
				m->normals[n + 2] = aiMesh->mNormals[v].z;
				n += 3;
			}

			if (aiMesh->mTextureCoords[0])
			{
				m->texCoords[tx] = aiMesh->mTextureCoords[0][v].x;
				m->texCoords[tx + 1] = aiMesh->mTextureCoords[0][v].y;
				tx += 2;
			}
			else
			{
				m->texCoords[tx] = 0.0f;
				m->texCoords[tx + 1] = 0.0f;
				tx += 2;
			}

			if (aiMesh->HasVertexColors(v))
			{
				m->colors[c] = aiMesh->mColors[v]->r;
				m->colors[c + 1] = aiMesh->mColors[v]->g;
				m->colors[c + 2] = aiMesh->mColors[v]->b;
				m->colors[c + 3] = aiMesh->mColors[v]->a;
				c += 4;
			}
			else
			{
				m->colors[c] = 0.0f;
				m->colors[c + 1] = 0.0f;
				m->colors[c + 2] = 0.0f;
				m->colors[c + 3] = 0.0f;
				c += 4;
			}
		}
		return m;
	}
	return nullptr;
}

Transform* ModuleImporter::LoadTransform(aiNode* n)
{
	aiVector3D aiPos, aiScale;
	aiQuaternion aiRot;

	n->mTransformation.Decompose(aiScale, aiRot, aiPos);

	float3 pos(aiPos.x, aiPos.y, aiPos.z);
	float3 scale(aiScale.x, aiScale.y, aiScale.z);
	Quat rot(aiRot.x, aiRot.y, aiRot.z, aiRot.w);

	while (strstr(n->mName.C_Str(), "_$AssimpFbx$_") != nullptr && n->mNumChildren == 1)
	{
		n = n->mChildren[0];

		n->mTransformation.Decompose(aiScale, aiRot, aiPos);

		float3 dummyPos(aiPos.x, aiPos.y, aiPos.z);
		float3 dummyScale(aiScale.x, aiScale.y, aiScale.z);
		Quat dummyRot(aiRot.x, aiRot.y, aiRot.z, aiRot.w);

		pos += dummyPos;
		rot = rot * dummyRot;
		scale = { scale.x * aiScale.x, scale.y * aiScale.y, scale.z * aiScale.z };
	}

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
			{
				*path = full.substr(0, pos_separator + 1).c_str();
			}
			else
			{
				path->clear();
			}
		}

		if (fileName != nullptr && pos_separator < full.length())
		{
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