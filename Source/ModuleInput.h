#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5
#define MAX_KEYS 300

enum class KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	ModuleInput(Application* app, bool startEnabled = true);
	~ModuleInput();

	bool Init();
	UpdateStatus PreUpdate();
	bool CleanUp();

	KEY_STATE GetKey(int id) const;
	KEY_STATE GetMouseButton(int id) const;
	int GetMouseX() const;
	int GetMouseY() const;
	int GetMouseZ() const;
	int GetMouseXMotion() const;
	int GetMouseYMotion() const;
	void LogInput(int id, KEY_STATE state);

public:
	bool isHovering;

private:
	KEY_STATE* keyboard;
	KEY_STATE mouseButtons[MAX_MOUSE_BUTTONS];
	int mouseX;
	int mouseY;
	int mouseZ;
	int mouseMotionX;
	int mouseMotionY;
};

#endif // !__MODULE_INPUT_H__