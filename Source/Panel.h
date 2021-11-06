#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"
#include <string>
#include <vector>

class Application;

class Panel
{
public:
	Panel(Application* parent, bool startEnabled = false) : App(parent), active(startEnabled)
	{}
	virtual ~Panel()
	{}

	virtual bool Start()
	{
		return true;
	}
	virtual UpdateStatus Update()
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}
	virtual bool RightClickMenu()
	{
		return true;
	}
	virtual bool CleanUp()
	{
		return true;
	}

public:
	Application* App;
	std::string name;
	bool active;
};

#endif // !__PANEL_H__