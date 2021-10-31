#ifndef __PANEL_H__
#define __PANEL_H__

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
		return update_status::UPDATE_CONTINUE;
	}

	virtual bool RightClickMenu()
	{
		return true;
	}

	virtual bool CleanUp()
	{
		return true;
	}
};

#endif // !__PANEL_H__