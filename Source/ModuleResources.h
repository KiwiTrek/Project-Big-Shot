#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "PanelResources.h"
#include "Resource.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"
#include "Color.h"

#include <map>

class Application;
class GameObject;
class aiMesh;


class ModuleResources : public Module
{
public:
	ModuleResources(Application* app, bool startEnabled = true);
	~ModuleResources();

	bool Init();
	bool CleanUp();

	UID GenerateNewUID();
	Resource* RequestResource(UID uid);

	//For loading purposes
	Resource* RequestResource(Shape shape);
	Resource* RequestResource(std::string name);
	Resource* RequestResource(Color color);

	const Resource* RequestResource(UID uid) const;
	Resource* GetShape(Shape shape);

	// returns -1 if the passed resource doesn't exist in the map
	UID Exists(Resource::Type type, const aiMesh& aiMesh);
	UID Exists(Resource::Type type, const char* pathFile, bool color = false, Color c = Color(1, 1, 1, 1));

	Resource* CreateNewResource(Resource::Type type, Shape shape = Shape::NONE, const char* pathFile = nullptr, bool color = false, Color c = Color(1, 1, 1, 1));
	void ReleaseResource(UID uid);
	bool SaveMesh(const ResourceMesh* rMesh, const std::string fileName);
	ResourceMesh* LoadMesh(std::string fileName);
	bool SaveMaterial(const ResourceMaterial* rMesh, const std::string fileName);
	ResourceMaterial* LoadMaterial(std::string fileName);

private:
	void ReleaseResourceReferences(Resource::Type type, UID uid, GameObject* parent = nullptr);

private:
	std::map<UID, Resource*> resources;
	std::map<UID, Resource*> loadedResources;
	std::map<Shape, Resource*> shapes;

	friend class PanelResources;
};

#endif // !__MODULE_RESOURCES_H__