#ifndef __PANEL_CONFIG_H__
#define __PANEL_CONFIG_H__

#include "Panel.h"

class PanelConfig :public Panel
{
public:
	PanelConfig(Application* app, bool startEnabled = false);
	~PanelConfig();

	bool Start();
	UpdateStatus Update();
	bool CleanUp();

	void UpdateHistogram();

public:
	std::vector<float> fpsHist;
	std::vector<float> msHist;

	//Update input
	bool update;

	//SDL_Version
	int sdlMajor, sdlMinor, sdlPatch;

	//Assimp Version
	int assimpMajor, assimpMinor, assimpPatch;

	//GL version
	std::string glVersion;

	//ImGui version
	std::string imguiVersion;

	//Glew version
	std::string glewVersion;

	//CPU
	int cpuCount, cpuSize;

	//RAM
	float ramSize;

	//Caps
	bool threeD, altiVec, avx, avx2, mmx, rdtsc, sse, sse2, sse3, sse41, sse42;

	//GPU
	uint gpuVendorId, gpuDeviceId;
	char gpuBrand[250];
	float videoMemBudget, videoMemCurrent, videoMemAvailable, videoMemReserved;
};

#endif // !__PANEL_CONFIG_H__