#include "Application.h"
#include "ModuleInput.h"

#include "ModuleWindow.h"
#include "ModuleFileSystem.h"
#include "ModuleImporter.h"
#include "ModuleResources.h"
#include "ModuleGameObjects.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"


ModuleInput::ModuleInput(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "input";
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, (int)KeyState::KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
	isHovering = false;
}

ModuleInput::~ModuleInput()
{
	delete[] keyboard;
	keyboard = nullptr;
}

bool ModuleInput::Init()
{
	LOG_CONSOLE("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG_CONSOLE("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

UpdateStatus ModuleInput::PreUpdate()
{
	SDL_PumpEvents();
	const Uint8* keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < MAX_KEYS; ++i)
	{
		if (keys[i] == 1)
		{
			if (keyboard[i] == KeyState::KEY_IDLE)
			{
				LogInput(i, KeyState::KEY_DOWN);
				keyboard[i] = KeyState::KEY_DOWN;
			}
			else if (keyboard[i] != KeyState::KEY_REPEAT)
			{
				LogInput(i, KeyState::KEY_REPEAT);
				keyboard[i] = KeyState::KEY_REPEAT;
			}
		}
		else
		{
			if (keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN)
			{
				LogInput(i, KeyState::KEY_UP);
				keyboard[i] = KeyState::KEY_UP;
			}
			else
			{
				keyboard[i] = KeyState::KEY_IDLE;
			}
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

	mouseX /= SCREEN_SIZE;
	mouseY /= SCREEN_SIZE;
	mouseZ = 0;

	for (int i = 0; i < 5; ++i)
	{
		if (buttons & SDL_BUTTON(i))
		{
			if (mouseButtons[i] == KeyState::KEY_IDLE)
			{
				LogInput(1000 + i, KeyState::KEY_DOWN);
				mouseButtons[i] = KeyState::KEY_DOWN;
			}
			else if (mouseButtons[i] != KeyState::KEY_REPEAT)
			{
				LogInput(1000 + i, KeyState::KEY_REPEAT);
				mouseButtons[i] = KeyState::KEY_REPEAT;
			}
		}
		else
		{
			if (mouseButtons[i] == KeyState::KEY_REPEAT || mouseButtons[i] == KeyState::KEY_DOWN)
			{
				LogInput(1000 + i, KeyState::KEY_UP);
				mouseButtons[i] = KeyState::KEY_UP;
			}
			else
			{
				mouseButtons[i] = KeyState::KEY_IDLE;
			}
		}
	}

	mouseMotionX = mouseMotionY = 0;

	bool quit = false;
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		App->gui->GetInput(&e);
		switch (e.type)
		{
		case SDL_MOUSEWHEEL:
		{
			mouseZ = e.wheel.y;
			break;
		}
		case SDL_MOUSEMOTION:
		{
			mouseX = e.motion.x / SCREEN_SIZE;
			mouseY = e.motion.y / SCREEN_SIZE;

			mouseMotionX = e.motion.xrel / SCREEN_SIZE;
			mouseMotionY = e.motion.yrel / SCREEN_SIZE;
			break;
		}
		case SDL_QUIT:
		{
			quit = true;
			break;
		}
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
			//App->fileSystem->Save(tmp.c_str(), e.drop.file, sizeof(e.drop.file));
			if (tmp.empty() != true)
			{
				if (tmp.find(".fbx") != std::string::npos || tmp.find(".FBX") != std::string::npos)
				{
					std::string fileName;
					App->fileSystem->SplitFilePath(tmp.c_str(), nullptr, &fileName);
					if (tmp.find(".FBX") == std::string::npos) fileName.append(".fbx");
					else fileName.append(".FBX");

					App->importer->ImportScene(tmp.c_str(), fileName.c_str());
				}
				else if (tmp.find(".png") != std::string::npos || tmp.find(".dds") != std::string::npos)
				{
					std::string fileName;
					App->fileSystem->SplitFilePath(tmp.c_str(), nullptr, &fileName);
					std::string pathName = fileName;
					if (tmp.find(".png") != std::string::npos) pathName.append(".png");
					else if (tmp.find(".dds") != std::string::npos) pathName.append(".dds");

					if (App->fileSystem->Exists(pathName))
					{
						UID uid = -1;
						uid = App->resources->Exists(Resource::Type::MATERIAL, fileName.c_str());
						if (App->gameObjects->selectedGameObject != nullptr && App->gameObjects->selectedGameObject != App->scene->GetSceneRoot())
						{
							ComponentMaterial* mat = App->gameObjects->selectedGameObject->GetComponent<Material>();

							if (mat != nullptr)
							{
								if (uid == -1)
								{
									mat->material = App->importer->LoadTexture(pathName.c_str());
									mat->material->name = fileName;
									mat->BindTexture(mat->usingCheckers);
								}
								else
								{
									mat->material = (ResourceMaterial*)App->resources->RequestResource(uid);
								}
							}
						}
						else
						{
							if (App->gameObjects->selectedGameObject != App->scene->GetSceneRoot())
							{
								LOG_CONSOLE("Error: Cannot assign material to scene root. Select object containing component Mesh");
							}
							else
							{
								LOG_CONSOLE("Error: No mesh component detected in selected Game Object. Consider selecting children");
							}
						}
					}
					else
					{
						LOG_CONSOLE("Error: Cannot assign material from outside Assets folder. Put it inside Assets/Resources/Materials");
					}
				}
				else
				{
					LOG_CONSOLE("File format selected not supported.");
				}
			}
			break;
		}
		}
	}

	if (quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KeyState::KEY_UP) return UpdateStatus::UPDATE_STOP;

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

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::LogInput(int id, KeyState state)
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
	App->gui->config->update = true;
}