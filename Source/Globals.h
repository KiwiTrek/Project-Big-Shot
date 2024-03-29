#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <windows.h>
#include <stdio.h>
#include "ConsoleBuffer.h"
#include "MathGeoLib.h"

#define MESH_FORMAT "bsm"
#define MESH_FORMAT_FILE ".bsm"
#define TEXTURE_FORMAT "bst"
#define TEXTURE_FORMAT_FILE ".bst"

#define LOAD_JSON_BOOL(b) { b = config.HasMember(#b) ? config[#b].GetBool() : b; }
#define SAVE_JSON_BOOL(b) { writer.String(#b); writer.Bool(b); }

#define LOAD_JSON_FLOAT(b) { b = config.HasMember(#b) ? config[#b].GetFloat() : b; }
#define SAVE_JSON_FLOAT(b) { writer.String(#b); writer.Double(b); }

#define LOG(format, ...) Log(__FILE__, __LINE__, format, __VA_ARGS__)

const char* Log(const char file[], int line, const char* format, ...);

#define LOG_CONSOLE(format, ...) \
	{\
		if (App->gui != nullptr && App->gui->console != nullptr) \
		{\
			App->gui->LogConsole(Log(__FILE__, __LINE__, format, __VA_ARGS__)); \
		}\
	}

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEG_TO_RAD 0.0174532925199432957f
#define RAD_TO_DEG 57.295779513082320876f

#define RELEASE( x )\
	{\
	   if( x != nullptr )\
	   {\
		 delete x;\
		 x = nullptr;\
	   }\
	}

#define RELEASE_ARRAY( x )\
	{\
	   if( x != nullptr )\
	   {\
		   delete[] x;\
		   x = nullptr;\
		 }\
	 }

typedef unsigned int UID;
typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef unsigned short ushort;

enum class UpdateStatus
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Particle
#define MAX_PARTICLES 20000

// ImGui
#define IMGUI_BLACK ImVec4(0.0f,0.0f,0.0f,1.f)
#define IMGUI_LIGHT_GREY ImVec4(0.84f,0.84f,0.84f,1.f)
#define IMGUI_GREY ImVec4(0.6f,0.6f,0.6f,1.f)
#define IMGUI_BLUE ImVec4(0.2f,0.2f,1.f,1.f)
#define IMGUI_GREEN ImVec4(0.f,1.f,0.f,1.f)
#define IMGUI_YELLOW ImVec4(1.f,1.f,0.f,1.f)
#define IMGUI_RED ImVec4(1.f,0.f,0.f,1.f)
#define IMGUI_WHITE ImVec4(1.f,1.f,1.f,1.f)

#define IMGUI_DARK_PINK ImVec4(0.33f,0.24f,0.33f,1.f)
#define IMGUI_PINK ImVec4(0.67f,0.23f,0.46f,1.f)
#define IMGUI_LIGHT_PINK ImVec4(0.8f,0.6f,0.7f,1.f)
#define IMGUI_DARK_PURPLE ImVec4(0.24f,0.2f,0.34f,1.f)
#define IMGUI_PURPLE ImVec4(0.45f,0.07f,0.69f,1.f)
#define IMGUI_LIGHT_PURPLE ImVec4(0.67f, 0.57f, 0.91f, 1.0f)
#define IMGUI_DARK_YELLOW ImVec4(0.75f,0.75f,0.f,1.0f)
#define IMGUI_YELLOW ImVec4(1.f,1.f,0.f,1.f)
#define IMGUI_LIGHT_GREEN ImVec4(0.36f, 0.62f, 0.58f, 1.0f)


#define IMGUI_PRINT(color, field, format, ...) \
	ImGui::Text(field); \
	ImGui::SameLine(); \
	ImGui::TextColored(color, format, __VA_ARGS__)

#define IMGUI_BULLET(color, field, format, ...) \
	ImGui::BulletText(field); \
	ImGui::SameLine(); \
	ImGui::TextColored(color, format, __VA_ARGS__)

// Timer
#define PERF_START(timer) timer.Start()
#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

// Configuration
#define SCREEN_SIZE 1
#define ASSETS_FOLDER "Assets/Resources/"
#define MODELS_FOLDER "Models/"
#define TEXTURES_FOLDER "Textures/"
#define SCENES_FOLDER "Assets/Scenes/"

#endif // !__GLOBALS_H__