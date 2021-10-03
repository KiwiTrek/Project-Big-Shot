#pragma once
#include "Globals.h"

class Application;
class ConfigJSON;

class Module
{
private :
	bool enabled;
public:
	Application* App;
	std::string name;

	Module(Application* parent, bool start_enabled = true) : App(parent)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual const char* GetName()
	{
		return name.c_str();
	}

	virtual bool Load(ConfigJSON* file)
	{
		return true;
	}

	virtual bool Save()
	{
		return true;
	}
};