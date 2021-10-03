#pragma once
#include "Globals.h"
#include <string>
#include <vector>

class Application;

class Panel
{
public:
	Application* App;
	std::string name;
	bool active;

	Panel(Application* parent, bool start_enabled = false) : App(parent), active(start_enabled)
	{}

	virtual ~Panel()
	{}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status Update()
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}
};