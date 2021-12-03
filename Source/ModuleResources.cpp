#include "Application.h"
#include "MathGeoLib.h"
#include "RenderGlobals.h"
#include "ModuleFileSystem.h"
#include "ModuleGameObjects.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"

#include "scene.h"
#include "postprocess.h"

ModuleResources::ModuleResources(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "resources";
}

ModuleResources::~ModuleResources()
{}

bool ModuleResources::Init()
{
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
	Resource* r = resources.at(uid);
	r->referenceCount++;
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