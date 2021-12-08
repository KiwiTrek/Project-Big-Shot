#include "PanelAbout.h"
#include "Application.h"
#include "ModuleGuiManager.h"

#include "ModuleInput.h"

PanelAbout::PanelAbout(Application* app, bool startEnabled) : Panel(app, startEnabled)
{
	name = "About";
}

PanelAbout::~PanelAbout()
{}

UpdateStatus PanelAbout::Update()
{
	App->input->isHovering = false;

	ImGui::Begin(name.c_str(), &active);
	ImGui::SetWindowFontScale(1.2f);
	ImGui::TextColored(IMGUI_WHITE, "Project"); ImGui::SameLine(); ImGui::TextColored(IMGUI_LIGHT_PINK, "Big"); ImGui::SameLine(); ImGui::TextColored(IMGUI_YELLOW, "Shot");
	ImGui::SetWindowFontScale(1);
	ImGui::Separator();
	ImGui::TextWrapped("Engine created for the 'Game Engines' subject in the Bachelor's degree in Video Game Design and Development at the CITM-UPC center, Barcelona.");
	ImGui::Text("By:");

	IMGUI_BULLET(IMGUI_YELLOW, "Guillem Alava ", "(https://github.com/WillyTrek19)");
	if (ImGui::IsItemHovered())
	{
		App->input->isHovering = true;
		ImGui::SetTooltip("Go to link");
	}
	if (ImGui::IsItemClicked()) App->RequestBrowser("https://github.com/WillyTrek19");

	IMGUI_BULLET(IMGUI_YELLOW, "Sergi Colomer ", "(https://github.com/Lladruc37)");
	if (ImGui::IsItemHovered())
	{
		App->input->isHovering = true;
		ImGui::SetTooltip("Go to link");
	}
	if (ImGui::IsItemClicked()) App->RequestBrowser("https://github.com/Lladruc37");

	ImGui::Separator();
	ImGui::TextWrapped("License:");
	ImGui::Spacing();
	ImGui::TextColored(IMGUI_YELLOW, "MIT License");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::TextWrapped("Copyright (c) 2021 Guillem Alava & Sergi Colomer");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::TextWrapped("Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::TextWrapped("The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::TextWrapped("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");

	ImGui::End();
	return UpdateStatus::UPDATE_CONTINUE;
}