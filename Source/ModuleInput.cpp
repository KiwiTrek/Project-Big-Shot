#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "input";
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, (int)KEY_STATE::KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	isHovering = false;
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG_CONSOLE("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG_CONSOLE("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if (keyboard[i] == KEY_STATE::KEY_IDLE)
			{
				LogInput(i, KEY_STATE::KEY_DOWN);
				keyboard[i] = KEY_STATE::KEY_DOWN;
			}
			else if(keyboard[i] != KEY_STATE::KEY_REPEAT)
			{
				LogInput(i, KEY_STATE::KEY_REPEAT);
				keyboard[i] = KEY_STATE::KEY_REPEAT;
			}
		}
		else
		{
			if (keyboard[i] == KEY_STATE::KEY_REPEAT || keyboard[i] == KEY_STATE::KEY_DOWN)
			{
				LogInput(i, KEY_STATE::KEY_UP);
				keyboard[i] = KEY_STATE::KEY_UP;
			}
			else
			{
				keyboard[i] = KEY_STATE::KEY_IDLE;
			}
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if (mouse_buttons[i] == KEY_STATE::KEY_IDLE)
			{
				LogInput(1000 + i, KEY_STATE::KEY_DOWN);
				mouse_buttons[i] = KEY_STATE::KEY_DOWN;
			}
			else if(mouse_buttons[i] != KEY_STATE::KEY_REPEAT)
			{
				LogInput(1000 + i, KEY_STATE::KEY_REPEAT);
				mouse_buttons[i] = KEY_STATE::KEY_REPEAT;
			}
		}
		else
		{
			if (mouse_buttons[i] == KEY_STATE::KEY_REPEAT || mouse_buttons[i] == KEY_STATE::KEY_DOWN)
			{
				LogInput(1000 + i, KEY_STATE::KEY_UP);
				mouse_buttons[i] = KEY_STATE::KEY_UP;
			}
			else
			{
				mouse_buttons[i] = KEY_STATE::KEY_IDLE;
			}
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			{
				mouse_z = e.wheel.y;
			}
			break;

			case SDL_MOUSEMOTION:
			{
				mouse_x = e.motion.x / SCREEN_SIZE;
				mouse_y = e.motion.y / SCREEN_SIZE;

				mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
				mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			}
			break;

			case SDL_QUIT:
			{
				quit = true;
			}
			break;

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					App->window->SetWidth(e.window.data1);
					App->window->SetHeight(e.window.data2);
				}
				break;
			}

			case SDL_DROPFILE:
			{
				std::string tmp;
				tmp.assign(e.drop.file);
				if (tmp.empty() != true)
				{
					if (tmp.find(".fbx") != std::string::npos)
					{
						std::string fileName = App->importer->GetFileName(tmp.c_str());
						App->importer->ImportScene(tmp.c_str(), fileName.c_str());
					}
					else if(tmp.find(".png") != std::string::npos)
					{
						if (App->scene->selectedGameObject != nullptr && App->scene->selectedGameObject != App->scene->GetSceneRoot())
						{
							Material* mat = nullptr;
							std::vector<Component*>::iterator c = App->scene->selectedGameObject->components.begin();
							while (c != App->scene->selectedGameObject->components.end())
							{
								if ((*c)->type == ComponentTypes::MATERIAL)
								{
									mat = (Material*)(*c);
								}
								c++;
							}

							if (mat != nullptr)
							{
								mat->SetTexture(App->importer->LoadTexture(tmp.c_str()));
							}
						}
						else
						{
							if (App->scene->selectedGameObject != App->scene->GetSceneRoot())
							{
								LOG_CONSOLE("Error: Cannot assign material to scene root. Select object containing component Mesh");
							}
							else
							{
								LOG_CONSOLE("Error: No mesh component detected in selected Game Object. Consider selecting children");
							}
						}
					}
				}
				break;
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_STATE::KEY_UP) return update_status::UPDATE_STOP;

	if (isHovering)
	{
		SDL_Cursor* cursor;
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		SDL_SetCursor(cursor);
	}
	else
	{
		SDL_FreeCursor(SDL_GetCursor());
	}

	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG_CONSOLE("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::LogInput(int id, KEY_STATE state)
{
	std::string tmp;
	static const char* states[] = { "IDLE", "DOWN", "REPEAT", "UP" };
	if (id < 1000)
	{
		tmp = "Keyboard: " + std::to_string(id) + " - "+ states[(int)state] + "\n";
	}
	else
	{
		tmp = "Mouse: " + std::to_string(id - 1000) + " - " + states[(int)state] + "\n";
	}
	App->gui->LogInputText.appendf(tmp.c_str());
}