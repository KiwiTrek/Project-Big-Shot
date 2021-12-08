#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"
#include "ModuleScene.h"
#include "ModuleGameObjects.h"
#include "ModuleResources.h"

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

void ModuleImporter::ImportScene(const char* path, const char* rootName)
{
	const aiScene* scene = nullptr;

	char* buffer = nullptr;
	uint bytesFile = App->fileSystem->Load(path, &buffer);

	if (buffer == nullptr)
	{
		std::string normPathShort = ASSETS_FOLDER;
		normPathShort.append(MODELS_FOLDER);
		normPathShort += App->fileSystem->SetNormalName(path);
		bytesFile = App->fileSystem->Load(normPathShort.c_str(), &buffer);
	}

	if (buffer != nullptr)
	{
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	}
	else
	{
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	}

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

	if (rootName != nullptr) g = new GameObject(rootName);
	else g = new GameObject(n->mName.C_Str());

	if (parentGameObject != nullptr)
	{
		parentGameObject->AddChild(g);
		g->parent = parentGameObject;
	}

	ComponentTransform* t = (ComponentTransform*)g->CreateComponent(ComponentTypes::TRANSFORM, LoadTransform(n));
	if (t->GetScale().x >= 100.0f) t->SetScale(1.0f, 1.0f, 1.0f);
	t->UpdateGlobalTransform();
	if (g->parent != nullptr) g->parent->UpdateChildrenTransforms();

	if (n->mMeshes != nullptr)
	{
		g->name = n->mName.C_Str();

		ComponentMaterial* mat = (ComponentMaterial*)g->CreateComponent(ComponentTypes::MATERIAL);
		mat->material = LoadTexture(scene, n);
		mat->BindTexture(mat->usingCheckers);

		ComponentMesh* mesh = (ComponentMesh*)g->CreateComponent(ComponentTypes::MESH);
		ResourceMesh* rMesh = ImportModel(scene, n);
		rMesh->GenerateBuffers();
		mesh->mesh = rMesh;
		if (mesh->mesh != nullptr)mesh->CreateBBox();
	}

	for (size_t i = 0; i < n->mNumChildren; i++)
	{
		ImportChild(scene, n->mChildren[i], n, g);
	}

	return g;
}

ResourceMaterial* ModuleImporter::LoadTexture(const char* path)
{
	std::string name;
	App->fileSystem->SplitFilePath(path, nullptr, &name);
	std::string newPath = App->fileSystem->GetPathRelativeToAssets(path);

	UID uid = App->resources->Exists(Resource::Type::MATERIAL, newPath.c_str());
	if (uid == -1)
	{
		uint id = 0;
		ilGenImages(1, &id);
		ilBindImage(id);

		ilEnable(IL_ORIGIN_SET);
		ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

		ResourceMaterial* texture = (ResourceMaterial*)App->resources->CreateNewResource(Resource::Type::MATERIAL, Shape::NONE, newPath.c_str());

		if (id == 0) LOG_CONSOLE("Error generation the image buffer: %s, %d", path, ilGetError());

		char* data;
		uint bytes = App->fileSystem->Load(path, &data);

		if (bytes != 0)
		{
			if (ilLoadL(IL_TYPE_UNKNOWN, data, bytes))
			{
				ILinfo ImageInfo;
				iluGetImageInfo(&ImageInfo);
				if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT) iluFlipImage();

				int channels = ilGetInteger(IL_IMAGE_CHANNELS);
				if (channels == 3) ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
				else if (channels == 4) ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

				ILenum error = ilGetError();

				if (error != IL_NO_ERROR)
				{
					LOG_CONSOLE("Material error: %d, %s", error, iluErrorString(error));
				}
				else
				{
					LOG_CONSOLE("Material loaded successfully from: %s", newPath.c_str());

					texture->texId = id;
					texture->name = name;
					texture->data = ilGetData();
					texture->width = ilGetInteger(IL_IMAGE_WIDTH);
					texture->height = ilGetInteger(IL_IMAGE_HEIGHT);
					texture->format = texture->formatUnsigned = ilGetInteger(IL_IMAGE_FORMAT);
					texture->path = newPath.c_str();
					std::string file = std::to_string(texture->GetUID()) + TEXTURE_FORMAT_FILE;
					//App->resources->SaveMaterial(texture, file);
				}
			}
			else
			{
				LOG_CONSOLE("Error loading the material from %s: %d, %s", path, ilGetError(), iluErrorString(ilGetError()));
			}
		}
		else
		{
			LOG_CONSOLE("Error loading material from %s: No bytes", path);
		}
		return texture;
	}
	else
	{
		ResourceMaterial* rm = (ResourceMaterial*)App->resources->RequestResource(uid);
		rm->referenceCount++;
		LOG_CONSOLE("Material already exists with uid : %d, pulling from resources.", (int)uid);
		return rm;
	}

	return nullptr;
}

ResourceMaterial* ModuleImporter::LoadTexture(const aiScene* scene, aiNode* n)
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

	ResourceMaterial* texture = nullptr;

	if (texPath.length != 0)
	{
		std::string newPath = App->fileSystem->GetPathRelativeToAssets(texPath.data);
		texture = LoadTexture(newPath.c_str());
		return texture;
	}
	else
	{
		LOG_CONSOLE("No texture found inside model, looking for a Color...");

		aiColor4D diffuse;
		if (material != nullptr && aiReturn::AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		{
			Color diff = Color(diffuse.r, diffuse.g, diffuse.b, diffuse.a);
			UID uid = App->resources->Exists(Resource::Type::MATERIAL, nullptr, false, diff);
			if (uid == -1)
			{
				texture = (ResourceMaterial*)App->resources->CreateNewResource(Resource::Type::MATERIAL,Shape::NONE, nullptr, true, diff);
				GenerateId(texture);
				std::string file = std::to_string(texture->GetUID()) + TEXTURE_FORMAT_FILE;
				//App->resources->SaveMaterial(texture, file);
				LOG_CONSOLE("Material color loaded successfully.");
			}
			else
			{
				ResourceMaterial* rm = (ResourceMaterial*)App->resources->RequestResource(uid);
				rm->referenceCount++;
				LOG_CONSOLE("Material color already exists with uid : %d, pulling from resources.", (int)uid);
				return rm;
			}
			return texture;
		}
		else
		{
			LOG_CONSOLE("No Color found!");
		}
	}


	return nullptr;
}

ResourceMesh* ModuleImporter::ImportModel(const aiScene* scene, aiNode* node)
{
	aiMesh* aiMesh = scene->mMeshes[*node->mMeshes];
	UID uid = App->resources->Exists(Resource::Type::MESH, *aiMesh);
	if (uid == -1)
	{
		ResourceMesh* m = (ResourceMesh*)App->resources->CreateNewResource(Resource::Type::MESH);
		m->vertexNum = aiMesh->mNumVertices;
		m->vertices.resize(aiMesh->mNumVertices);
		memcpy(&m->vertices[0], aiMesh->mVertices, sizeof(float3) * m->vertexNum);
		LOG_CONSOLE("Loaded new mesh with %d vertices", m->vertexNum);

		if (aiMesh->HasFaces())
		{
			m->indexNum = aiMesh->mNumFaces * 3;
			m->indices.resize(m->indexNum);
			for (uint j = 0; j < aiMesh->mNumFaces; j++)
			{
				if (aiMesh->mFaces[j].mNumIndices != 3) LOG_CONSOLE("ERROR: Geometry face with != 3 indices!")
				else memcpy(&m->indices[j * 3], aiMesh->mFaces[j].mIndices, 3 * sizeof(uint));
			}

			if (aiMesh->HasNormals())
			{
				m->normalNum = aiMesh->mNumVertices;
				m->normals.resize(m->normalNum);
				memcpy(&m->normals[0], aiMesh->mNormals, sizeof(float3) * m->normalNum);
			}

			m->texCoords.resize(aiMesh->mNumVertices);
			if (aiMesh->HasTextureCoords(0))
			{
				for (size_t j = 0; j < aiMesh->mNumVertices; ++j)
				{
					memcpy(&m->texCoords[j], &aiMesh->mTextureCoords[0][j], sizeof(float2));
				}
			}
			else
			{
				for (size_t j = 0; j < aiMesh->mNumVertices; ++j)
				{
					m->texCoords.at(j).x = 0.0f;
					m->texCoords.at(j).y = 0.0f;
				}
			}
			std::string file = std::to_string(m->GetUID()) + MESH_FORMAT_FILE;
			App->resources->SaveMesh(m, file);
			return m;
		}
	}
	else
	{
		ResourceMesh* rm = (ResourceMesh*)App->resources->RequestResource(uid);
		rm->referenceCount++;
		return rm;
	}
	return nullptr;
}

ComponentTransform* ModuleImporter::LoadTransform(aiNode* n)
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

	ComponentTransform* t = new ComponentTransform();
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

void ModuleImporter::GenerateId(ResourceMaterial* rm)
{
	uint id = 0;
	ilGenImages(1, &id);
	ilBindImage(id);
	rm->texId = id;
}