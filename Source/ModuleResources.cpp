#include "Application.h"
#include "MathGeoLib.h"
#include "RenderGlobals.h"
#include "ModuleFileSystem.h"
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
	resources.erase(uid);
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
			rm->path = pathFile;
		}

		if (rm != nullptr)
		{
			rm->SetResourceMap(&resources);
			resources[uid] = rm;
			r = (Resource*)rm;
			r->referenceCount++;
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
		r->referenceCount++;
		break;
	}
	}

	return r;
}