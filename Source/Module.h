#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"
#include <string>

class Application;

class Module
{
private :
	bool enabled;
public:
	Application* App;
	std::string name;

	Module(Application* parent, bool start_enabled = true) : App(parent), enabled(start_enabled)
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
		return update_status::UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate()
	{
		return update_status::UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual const char* GetName()
	{
		return name.c_str();
	}
};

#endif // !__MODULE_H__