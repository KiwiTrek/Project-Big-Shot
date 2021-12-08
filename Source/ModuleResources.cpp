#include "Application.h"
#include "MathGeoLib.h"
#include "RenderGlobals.h"
#include "ModuleFileSystem.h"
#include "ModuleGameObjects.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"

#include "Timer.h"
#include "scene.h"
#include "postprocess.h"
#include <iostream>
#include <fstream>

ModuleResources::ModuleResources(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "resources";
}

ModuleResources::~ModuleResources()
{}

bool ModuleResources::Init()
{
	LOG_CONSOLE("Loading models found in Resources folder");
	std::vector<std::string> listNames;
	App->fileSystem->GetAllFilesWithExtension(App->fileSystem->meshPath.c_str(), MESH_FORMAT, listNames);
	for (uint i = 0; i < listNames.size(); ++i)
	{
		ResourceMesh* rm = LoadMesh(listNames.at(i).c_str());
		if (rm != nullptr) loadedResources[rm->GetUID()] = rm;
	}

	LOG_CONSOLE("Loading materials found in Resources folder");
	listNames.clear();
	App->fileSystem->GetAllFilesWithExtension(App->fileSystem->texturePath.c_str(), TEXTURE_FORMAT, listNames);
	for (uint i = 0; i < listNames.size(); ++i)
	{
		ResourceMaterial* rm = LoadMaterial(listNames.at(i).c_str());
		if (rm != nullptr) loadedResources[rm->GetUID()] = rm;
	}

	shapes[Shape::CUBE] = CreateNewResource(Resource::Type::MESH, Shape::CUBE);
	shapes[Shape::SPHERE] = CreateNewResource(Resource::Type::MESH, Shape::SPHERE);
	shapes[Shape::CYLINDER] = CreateNewResource(Resource::Type::MESH, Shape::CYLINDER);
	shapes[Shape::TORUS] = CreateNewResource(Resource::Type::MESH, Shape::TORUS);
	shapes[Shape::PLANE] = CreateNewResource(Resource::Type::MESH, Shape::PLANE);
	shapes[Shape::CONE] = CreateNewResource(Resource::Type::MESH, Shape::CONE);
	return true;
}

bool ModuleResources::CleanUp()
{
	resources.clear();
	shapes.clear();
	return true;
}

UID ModuleResources::Exists(Resource::Type type, const aiMesh& aiMesh)
{
	UID uid = -1;

	if (type == Resource::Type::MESH)
	{
		for (std::map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		{
			ResourceMesh* rm = (ResourceMesh*)(*it).second;
			if ((*it).second->GetType() == Resource::Type::MESH)
			{
				if (rm->vertexNum == aiMesh.mNumVertices
					&& memcmp(&rm->normals[0], aiMesh.mNormals, sizeof(float3) * aiMesh.mNumVertices) == 0)
				{
					return (*it).first;
				}
			}
		}
		for (std::map<UID, Resource*>::iterator it = loadedResources.begin(); it != loadedResources.end(); ++it)
		{
			ResourceMesh* rm = (ResourceMesh*)(*it).second;
			if ((*it).second->GetType() == Resource::Type::MESH)
			{
				if (rm->vertexNum == aiMesh.mNumVertices
					&& memcmp(&rm->normals[0], aiMesh.mNormals, sizeof(float3) * aiMesh.mNumVertices) == 0)
				{
					return (*it).first;
				}
			}
		}
	}

	return uid;
}

UID ModuleResources::Exists(Resource::Type type, const char* pathFile, bool color, Color c)
{
	UID uid = -1;

	if (type == Resource::Type::MATERIAL)
	{
		for (std::map<UID, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		{
			ResourceMaterial* rm = (ResourceMaterial*)(*it).second;
			if ((*it).second->GetType() == Resource::Type::MATERIAL)
			{
				if (color && rm->diffuse == c)
				{
					return (*it).first;
				}
				else if (pathFile != nullptr && pathFile == rm->path)
				{
					return (*it).first;
				}
			}
		}
	}

	return uid;
}


UID ModuleResources::GenerateNewUID()
{
	UID uid  = LCG().Int();
	return uid;
}

const Resource* ModuleResources::RequestResource(UID uid) const
{
	Resource* r = resources.at(uid);
	r->referenceCount++;
	return r;
}

Resource* ModuleResources::GetShape(Shape shape)
{
	Resource* r = shapes.at(shape);
	r->referenceCount++;
	return shapes.at(shape);
}

Resource* ModuleResources::RequestResource(UID uid)
{
	Resource* r = nullptr;
	if (resources.find(uid) == resources.end())
	{
		if (loadedResources.find(uid) == loadedResources.end())
		{
			LOG("HAHA NO FUNCIONA");
		}
		else
		{
			r = loadedResources.at(uid);
			if (r->GetType() == Resource::Type::MATERIAL)
			{
				App->importer->GenerateId((ResourceMaterial*)r);
			}
			r->SetResourceMap(&resources);
			resources[uid] = r;
		}
	}
	else
	{
		r = resources.at(uid);
	}
	if(r != nullptr) r->referenceCount++;
	return r;
}

Resource* ModuleResources::RequestResource(Shape shape)
{
	Resource* r = nullptr;
	std::map<Shape, Resource*>::iterator it = shapes.begin();
	while (it != shapes.end())
	{
		if ((*it).first == shape)
		{
			ResourceMesh* rMesh = (ResourceMesh*)(*it).second;
			rMesh->mType = shape;
			rMesh->GenerateBuffers();
			r = rMesh;
			r->SetUID((*it).second->GetUID());
			break;
		}
		it++;
	}

	return r;
}

Resource* ModuleResources::RequestResource(std::string name)
{
	Resource* r = nullptr;
	std::map<UID, Resource*>::iterator it = loadedResources.begin();
	while (it != loadedResources.end())
	{
		if ((*it).second->GetAssetFile() == name)
		{
			r = (*it).second;
			r->SetUID((*it).first);
			break;
		}
		it++;
	}

	if (r == nullptr)
	{
		std::map<UID, Resource*>::iterator it = resources.begin();
		while (it != resources.end())
		{
			if ((*it).second->GetAssetFile() == name)
			{
				r = (*it).second;
				r->SetUID((*it).first);
				break;
			}
			it++;
		}
	}

	return r;
}

Resource* ModuleResources::RequestResource(Color color)
{
	Resource* r = nullptr;
	std::map<UID, Resource*>::iterator it = loadedResources.begin();
	while (it != loadedResources.end())
	{
		ResourceMaterial* rColor = (ResourceMaterial*)(*it).second;
		if (rColor->diffuse == color)
		{
			r = (*it).second;
			r->SetUID((*it).first);
			break;
		}
		it++;
	}

	if (r == nullptr)
	{
		std::map<UID, Resource*>::iterator it = resources.begin();
		while (it != resources.end())
		{
			ResourceMaterial* rColor = (ResourceMaterial*)(*it).second;
			if (rColor->diffuse == color)
			{
				r = (*it).second;
				r->SetUID((*it).first);
				break;
			}
			it++;
		}
	}

	return r;
}

void ModuleResources::ReleaseResource(UID uid)
{
	Resource* r = resources.at(uid);
	r->referenceCount--;
	ReleaseResourceReferences(r->GetType(), uid);
	resources.erase(resources.find(uid));
	if (r != nullptr) delete r;
	r = nullptr;
}

void ModuleResources::ReleaseResourceReferences(Resource::Type type, UID uid, GameObject* parent)
{
	std::vector<GameObject*>::iterator it;
	parent != nullptr ? it = parent->children.begin() : it = App->gameObjects->gameObjectList.begin();

	std::vector<GameObject*>::iterator ed;
	parent != nullptr ? ed = parent->children.end() : ed = App->gameObjects->gameObjectList.end();

	while (it != ed)
	{
		if (!(*it)->children.empty()) ReleaseResourceReferences(type, uid, (*it));

		switch (type)
		{
		case Resource::Type::MATERIAL:
		{
			ComponentMaterial* c = (*it)->GetComponent<Material>();
			if (c != nullptr && c->material != nullptr)
			{
				if (c->material->GetUID() == uid) c->material = nullptr;
			}
			break;
		}
		case Resource::Type::MESH:
		{
			ComponentMesh* c = (*it)->GetComponent<Mesh>();
			if (c != nullptr && c->mesh != nullptr)
			{
				if (c->mesh->GetUID() == uid) c->mesh = nullptr;
			}
			break;
		}
		}

		++it;
	}
}

Resource* ModuleResources::CreateNewResource(Resource::Type type,Shape shape, const char* pathFile, bool color, Color c)
{
	Resource* r = nullptr;
	UID uid = GenerateNewUID();

	switch (type)
	{
	case Resource::Type::MATERIAL:
	{
		ResourceMaterial* rm = nullptr;

		if (color)
		{
			rm = new ResourceMaterial(uid, c);
		}
		else
		{
			rm = new ResourceMaterial(uid);
			rm->SetAssetFile(pathFile);
			std::string newPath = ASSETS_FOLDER;
			newPath.append(TEXTURES_FOLDER);
			newPath.append(pathFile);
			rm->path = newPath;
		}

		if (rm != nullptr)
		{
			rm->SetResourceMap(&resources);
			resources[uid] = rm;
			r = (Resource*)rm;
		}
		break;
	}
	case Resource::Type::MESH:
	{
		ResourceMesh* rm = nullptr;
		shape == Shape::NONE ? rm = new ResourceMesh(uid) : rm = new ResourceMesh(uid, shape);
		r = (Resource*)rm;
		r->SetResourceMap(&resources);
		resources[uid] = r;
		break;
	}
	}

	if (r != nullptr) r->referenceCount++;
	return r;
}

bool ModuleResources::SaveMesh(const ResourceMesh* rMesh, const std::string fileName)
{
	std::ofstream myFile;
	std::string path = App->fileSystem->meshPath + "/";
	path.append(fileName);
	myFile.open(path.c_str(), std::ios::in | std::ios::app | std::ios::binary);
	if (myFile.is_open())
	{
		uint ranges[4] = {rMesh->GetUID(), rMesh->vertexNum, rMesh->normalNum, rMesh->indexNum };
		uint vertexBytes = sizeof(float) * rMesh->vertexNum * 3;
		uint normalBytes = sizeof(float) * rMesh->normalNum * 3;
		uint texCoordsBytes = sizeof(float) * rMesh->vertexNum * 2;
		uint indexBytes = sizeof(uint) * rMesh->indexNum;
		uint size = sizeof(ranges) + vertexBytes + normalBytes + texCoordsBytes + indexBytes;

		char* fileBuff = new char[size];
		char* cursor = fileBuff;

		uint bytes = sizeof(ranges);
		memcpy(cursor, ranges, bytes);
		cursor += bytes;

		bytes = vertexBytes;
		memcpy(cursor, rMesh->vertices.data(), bytes);
		cursor += bytes;

		bytes = normalBytes;
		memcpy(cursor, rMesh->normals.data(), bytes);
		cursor += bytes;

		bytes = texCoordsBytes;
		memcpy(cursor, rMesh->texCoords.data(), bytes);
		cursor += bytes;

		bytes = indexBytes;
		memcpy(cursor, rMesh->indices.data(), bytes);
		cursor += bytes;

		myFile.write(fileBuff, size);

		myFile.close();
		return true;
	}
	return false;
}

ResourceMesh* ModuleResources::LoadMesh(std::string fileName)
{
	Timer timer;
	timer.Start();

	std::ifstream myFile;
	std::string path = App->fileSystem->meshPath + "/";
	path.append(fileName);
	myFile.open(path.c_str(), std::ios::binary);
	if (myFile.is_open())
	{
		UID uid = GenerateNewUID();
		ResourceMesh* rMesh = new ResourceMesh(uid);

		myFile.seekg(0, myFile.end);
		int length = myFile.tellg();
		myFile.seekg(0, myFile.beg);

		char* fileBuff = new char[length];
		myFile.read(fileBuff, length);

		char* cursor = fileBuff;
		uint ranges[4];

		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		cursor += bytes;

		rMesh->SetUID(ranges[0]);
		rMesh->vertexNum = ranges[1];
		rMesh->normalNum = ranges[2];
		rMesh->indexNum = ranges[3];

		bytes = sizeof(float) * rMesh->vertexNum * 3;
		float* v = new float[rMesh->vertexNum*3];
		memcpy(v, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(float) * rMesh->normalNum*3;
		float* n = new float[rMesh->normalNum*3];
		memcpy(n, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(float) * rMesh->vertexNum*2;
		float* tx = new float[rMesh->vertexNum*2];
		memcpy(tx, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(uint) * rMesh->indexNum;
		uint* ind = new uint[rMesh->indexNum];
		memcpy(ind, cursor, bytes);
		cursor += bytes;
	
		myFile.close();

		rMesh->vertices.resize(rMesh->vertexNum);
		rMesh->normals.resize(rMesh->normalNum);
		rMesh->texCoords.resize(rMesh->vertexNum);
		rMesh->indices.resize(rMesh->indexNum);

		for (uint i = 0; i < rMesh->vertexNum; ++i)
		{
			memcpy(&rMesh->vertices[i], &v[i * 3], sizeof(float) * 3);
			memcpy(&rMesh->texCoords[i], &tx[i * 2], sizeof(float) * 2);
		}
		for (uint i = 0; i < rMesh->normalNum; ++i)
		{
			memcpy(&rMesh->normals[i], &n[i * 3], sizeof(float) * 3);
		}
		for (uint i = 0; i < rMesh->indexNum; ++i)
		{
			memcpy(&rMesh->indices[i], &ind[i], sizeof(uint));
		}

		LOG_CONSOLE("Mesh loaded successfully in %f ms", timer.ReadSec());
		return rMesh;
	}
	return nullptr;
}

bool ModuleResources::SaveMaterial(const ResourceMaterial* rm, const std::string fileName)
{
	std::ofstream myFile;
	std::string path = App->fileSystem->texturePath + "/";
	path.append(fileName);
	myFile.open(path.c_str(), std::ios::in | std::ios::app | std::ios::binary);
	if (myFile.is_open())
	{
		uint ranges[5] = { rm->GetUID(), rm->width, rm->height, rm->format, rm->formatUnsigned };
		uint nameBytes = sizeof(char) * 128;
		uint pathBytes = sizeof(char) * 128;
		uint dataBytes = sizeof(GLubyte) * 960 * 540;
		uint colorBytes = sizeof(float) * 4;
		uint size = sizeof(ranges) + nameBytes + pathBytes + dataBytes + colorBytes;

		char* fileBuff = new char[size];
		char* cursor = fileBuff;

		uint bytes = sizeof(ranges);
		memcpy(cursor, ranges, bytes);
		cursor += bytes;

		bytes = nameBytes;
		memcpy(cursor, rm->name.c_str(), bytes);
		cursor += bytes;

		bytes = pathBytes;
		memcpy(cursor, rm->path.c_str(), bytes);
		cursor += bytes;

		bytes = dataBytes;
		memcpy(cursor, rm->data, bytes); // peta aqui
		cursor += bytes;

		bytes = colorBytes;
		float color[4] = { rm->diffuse.r,rm->diffuse.g,rm->diffuse.b,rm->diffuse.a };
		memcpy(cursor, color, bytes);
		cursor += bytes;

		myFile.write(fileBuff, size);

		myFile.close();
		return true;
	}
	return false;
}

ResourceMaterial* ModuleResources::LoadMaterial(std::string fileName)
{
	Timer timer;
	timer.Start();

	std::ifstream myFile;
	std::string path = App->fileSystem->texturePath + "/";
	path.append(fileName);
	myFile.open(path.c_str(), std::ios::binary);
	if (myFile.is_open())
	{
		UID uid = GenerateNewUID();
		ResourceMaterial* rm = new ResourceMaterial(uid);

		myFile.seekg(0, myFile.end);
		int length = myFile.tellg();
		myFile.seekg(0, myFile.beg);

		char* fileBuff = new char[length];
		myFile.read(fileBuff, length);

		char* cursor = fileBuff;
		uint ranges[5];

		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		cursor += bytes;

		rm->SetUID(ranges[0]);
		rm->width = ranges[1];
		rm->height = ranges[2];
		rm->format = ranges[3];
		rm->formatUnsigned = ranges[4];

		bytes = sizeof(char) * 128;
		char* name = new char[128];
		memcpy(name, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(char) * 128;
		char* path = new char[128];
		memcpy(path, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(GLubyte) * 960 * 540;
		GLubyte* data = new GLubyte[960 * 540];
		memcpy(data, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(float) * 4;
		float* color = new float[4];
		memcpy(color, cursor, bytes);
		cursor += bytes;

		myFile.close();

		rm->name = name;
		rm->path = path;
		rm->data = data;
		rm->diffuse = Color(color[0], color[1], color[2], color[3]);

		LOG_CONSOLE("Material loaded successfully in %f ms", timer.ReadSec());
		return rm;
	}
	return nullptr;
}