#include <stdlib.h>
#include "ConsoleBuffer.h"
#include "Globals.h"
#include "Application.h"
#include "SDL.h"

enum class MainStates
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char** argv)
{
	ConsoleBuffer* consoleBuff = new ConsoleBuffer();
	consoleBuff->initBuff = LOG("Starting Engine...");

	int mainReturn = EXIT_FAILURE;
	MainStates state = MainStates::MAIN_CREATION;
	Application* App = NULL;

	while (state != MainStates::MAIN_EXIT)
	{
		switch (state)
		{
		case MainStates::MAIN_CREATION:
		{
			consoleBuff->initBuff2 = LOG("-------------- Application Creation --------------");
			App = new Application(consoleBuff);
			state = MainStates::MAIN_START;
			break;
		}
		case MainStates::MAIN_START:
		{
			LOG_CONSOLE("-------------- Application Init --------------");
			if (App->Init() == false)
			{
				LOG_CONSOLE("Application Init exits with ERROR");
				state = MainStates::MAIN_EXIT;
			}
			else
			{
				state = MainStates::MAIN_UPDATE;
				LOG_CONSOLE("-------------- Application Update --------------");
			}
			break;
		}
		case MainStates::MAIN_UPDATE:
		{
			UpdateStatus updateReturn = App->Update();

			if (updateReturn == UpdateStatus::UPDATE_ERROR)
			{
				LOG_CONSOLE("Application Update exits with ERROR");
				state = MainStates::MAIN_EXIT;
			}

			if (updateReturn == UpdateStatus::UPDATE_STOP) state = MainStates::MAIN_FINISH;

			break;
		}
		case MainStates::MAIN_FINISH:
		{
			LOG_CONSOLE("-------------- Application CleanUp --------------");

			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with ERROR");
			}
			else
			{
				mainReturn = EXIT_SUCCESS;
			}

			state = MainStates::MAIN_EXIT;
			break;
		}
		}
	}

	delete App;
	LOG("Exiting Engine...\n");
	return mainReturn;
}