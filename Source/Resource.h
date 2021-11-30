#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "Globals.h"
#include <map>

class Resource
{
public:
	enum class Type
	{
		MATERIAL,
		MESH,
		NONE
	};

	Resource(UID uid, Resource::Type type) :uid(uid), type(type), resources(nullptr) {}
	virtual ~Resource() {}
	Resource::Type GetType() const { return type; }
	UID GetUID() const { return uid; }
	const char* GetAssetFile() const { return assetsFile.c_str(); }
	void SetResourceMap(std::map<UID, Resource*>* map) { resources = map; }
	std::map<UID, Resource*>* GetResourceMap() { return resources; }

public:
	uint referenceCount = 0;

protected:
	UID uid;
	Type type;
	std::string assetsFile;
	std::map<UID, Resource*>* resources = nullptr;
};

#endif // !__RESOURCE_H__