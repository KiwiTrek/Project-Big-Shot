#ifndef __MODULE_H__
#define __MODULE_H__

#include "Globals.h"
#include <string>

class Application;

class Module
{
public:
	Module(Application* parent, bool startEnabled = true) : App(parent), enabled(startEnabled)
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
	virtual UpdateStatus PreUpdate()
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}
	virtual UpdateStatus Update(float dt)
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}
	virtual UpdateStatus PostUpdate()
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}
	virtual bool CleanUp()
	{
		return true;
	}
	virtual const char* GetName()
	{
		return name.c_str();
	}

public:
	Application* App;

protected:
	std::string name;

private:
	bool enabled;
};

#endif // !__MODULE_H__