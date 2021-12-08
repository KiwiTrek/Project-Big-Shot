#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5
#define MAX_KEYS 300

enum class KeyState
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

	KeyState GetKey(int id) const { return keyboard[id]; };
	KeyState GetMouseButton(int id) const { return mouseButtons[id]; };
	int GetMouseX() const { return mouseX; }
	int GetMouseY() const { return mouseY; }
	int GetMouseZ() const { return mouseZ; }
	int GetMouseXMotion() const { return mouseMotionX; }
	int GetMouseYMotion() const { return mouseMotionY; }
	void LogInput(int id, KeyState state);

public:
	bool isHovering;

private:
	KeyState* keyboard;
	KeyState mouseButtons[MAX_MOUSE_BUTTONS];
	int mouseX;
	int mouseY;
	int mouseZ;
	int mouseMotionX;
	int mouseMotionY;
};

#endif // !__MODULE_INPUT_H__