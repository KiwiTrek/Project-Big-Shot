#pragma once
#include <windows.h>
#include <stdio.h>
#include <glew.h>
#include <vector>
#include <string>
#include "ConsoleBuffer.h"

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__)

const char* log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

typedef unsigned int uint;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef unsigned long long UID;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// ImGui Globals
#define IMGUI_BLACK ImVec4(0.0f,0.0f,0.0f,1.f)
#define IMGUI_LIGHT_GREY ImVec4(0.8f,0.8f,0.8f,1.f)
#define IMGUI_GREY ImVec4(0.6f,0.6f,0.6f,1.f)
#define IMGUI_BLUE ImVec4(0.2f,0.2f,1.f,1.f)
#define IMGUI_GREEN ImVec4(0.f,1.f,0.f,1.f)
#define IMGUI_YELLOW ImVec4(1.f,1.f,0.f,1.f)
#define IMGUI_RED ImVec4(1.f,0.f,0.f,1.f)
#define IMGUI_WHITE ImVec4(1.f,1.f,1.f,1.f)
#define IMGUI_PRINT(color, field, format, ...) \
    ImGui::Text(field); \
    ImGui::SameLine(); \
    ImGui::TextColored(color, format, __VA_ARGS__)

#define IMGUI_BULLET(color, field, format, ...) \
    ImGui::BulletText(field); \
    ImGui::SameLine(); \
    ImGui::TextColored(color, format, __VA_ARGS__)

// Performance macros
#define PERF_START(timer) timer.Start()
#define PERF_PEEK(timer) LOG("%s took %f ms", __FUNCTION__, timer.ReadMs())

// Configuration -----------
#define SCREEN_SIZE 1