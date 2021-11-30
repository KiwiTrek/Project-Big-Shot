#ifndef __MODULE_RESOURCES_H__
#define __MODULE_RESOURCES_H__

#include "Module.h"
#include "Resource.h"
#include "ResourceMaterial.h"
#include "ResourceMesh.h"

#include <map>

class Application;
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
	const Resource* RequestResource(UID uid) const;
	Resource* GetShape(Shape shape);

	// returns -1 if the passed resource doesn't exist in the map
	UID Exists(Resource::Type type, const aiMesh& aiMesh);
	UID Exists(Resource::Type type, const char* pathFile, bool color = false, Color c = Color(1, 1, 1, 1));

	Resource* CreateNewResource(Resource::Type type, Shape shape = Shape::NONE, const char* pathFile = nullptr, bool color = false, Color c = Color(1, 1, 1, 1));
	void ReleaseResource(UID uid);

private:
	std::map<UID, Resource*> resources;
	std::map<Shape, Resource*> shapes;
};

#endif // !__MODULE_RESOURCES_H__