#include "GameObject.h"
#include "RenderGlobals.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleParticles.h"
#include "ResourceMaterial.h"
#include "ModuleGameObjects.h"

ComponentEmitter::ComponentEmitter(EmitterData data, bool active) : Component(type, active), data(data)
{
    type = ComponentTypes::EMITTER;
	particlePool.clear();

	lifeTimer.Start();
	burstTimer.Start();
	loopTimer.Start();

	drawingBbox.SetFromCenterAndSize(vec(0.0f, 0.0f, 0.0f), vec(1.1f, 1.1f, 1.1f));
}

ComponentEmitter::~ComponentEmitter()
{
	particlePool.clear();
}

void ComponentEmitter::StartEmitter()
{
	lifeTimer.Start();
	burstTimer.Start();
	loopTimer.Start();

	data.timeToParticle = 0.0f;
}

void ComponentEmitter::Update(float dt, Application* App)
{
	bbox.SetFromCenterAndSize(owner->GetComponent<ComponentTransform>()->GetPos(), vec(1.1f, 1.1f, 1.1f));

	float3 defaultPos = float3(0.5, 1.0, -0.5);
	float time = lifeTimer.ReadSec();
	if (time > data.timeToParticle && (data.loop || loopTimer.ReadSec() < data.duration))
	{
		float newTimeToParticle = 1.0f / data.rateOverTime;
		int num = (time / newTimeToParticle);
		CreateParticles(num, defaultPos);
		data.timeToParticle = newTimeToParticle;
		lifeTimer.Start();
	}

	float burstTime = burstTimer.ReadSec();
	if (data.burst && burstTime > data.repeatTime)
	{
		int num = data.minPart;
		if (data.minPart != data.maxPart)
		{
			num = (rand() % (data.maxPart - data.minPart)) + data.minPart;
		}
		CreateParticles(num, defaultPos);
		burstTimer.Start();
	}

	if (!newPositions.empty())
	{
		for (std::vector<float3>::iterator it = newPositions.begin(); it != newPositions.end(); ++it)
		{
			CreateParticles(data.subRateParticles, (*it), true);
		}
		newPositions.clear();
	}

	// Update all alive particles
	for (std::vector<Particle*>::iterator p = particlePool.begin(); p != particlePool.end(); ++p)
	{
		if (!(*p)->active)
		{
			DestroyParticle((*p));
			p = particlePool.erase(p);
			if (p != particlePool.begin()) --p;
			else if (p == particlePool.end()) break;
		}
	}

	for (std::vector<Particle*>::iterator p = particlePool.begin(); p != particlePool.end(); ++p)
	{
		(*p)->Update(dt, App);
	}
}

void ComponentEmitter::PostUpdate()
{
	float sx = 0.5f;
	float sy = 0.5f;
	float sz = 0.5f;

	//Draw Body
	float4x4 t = owner->GetComponent<Transform>()->GetGlobalTransform();
	glPushMatrix();
	glMultMatrixf((float*)&t.Transposed());

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.5f);
	glBegin(GL_QUADS);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(-sx, sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, sy, sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(-sx, -sy, sz);

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	glPopMatrix();
}

void ComponentEmitter::DrawBbox() const
{
	float4x4 t = owner->GetComponent<Transform>()->GetGlobalTransform();

	glPushMatrix();
	glMultMatrixf((float*)&t.Transposed());

	float3 cornerPoints[8];
	drawingBbox.GetCornerPoints(cornerPoints);

	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	glLineWidth(3.5f);
	glBegin(GL_LINES);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	glPopMatrix();
}

void ComponentEmitter::DrawInspector(Application* App)
{
	if (ImGui::CollapsingHeader("Particle System", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::TreeNodeEx("Particle Values"))
		{
			//this is test
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted("Active checkBox if you want a random number");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::Checkbox("##Speed", &data.checkSpeed);
			ShowFloatValue(data.speed, data.checkSpeed, "Speed", 0.25f, 0.25f, 20.0f);

			ImGui::Checkbox("##Acceleration", &data.checkAcceleration);
			ShowFloatValue(data.acceleration, data.checkAcceleration, "Acceleration", 0.25f, -5.0f, 5.0f);

			ImGui::Checkbox("##Rotation", &data.checkRotation);
			ShowFloatValue(data.rotation, data.checkRotation, "Initial Rotation", 0.25f, -360.0f, 360.0f);

			ImGui::Checkbox("##AngVelocity", &data.checkAngularVelocity);
			ShowFloatValue(data.angularVelocity, data.checkAngularVelocity, "Angular Vel.", 0.25f, -45.0f, 45.0f);

			ImGui::Checkbox("##AngAcceleration", &data.checkAngularAcceleration);
			ShowFloatValue(data.angularAcceleration, data.checkAngularAcceleration, "Angular Accel.", 0.25f, -45.0f, 45.0f);

			ImGui::Checkbox("##Lifetime", &data.checkLife);
			ShowFloatValue(data.particleLife, data.checkLife, "Life", 0.5f, 1.0f, 20.0f);

			ImGui::Checkbox("##Size", &data.checkSize);
			ShowFloatValue(data.size, data.checkSize, "Size", 0.1f, 0.1f, 5.0f);

			ImGui::Checkbox("##SizeOverTime", &data.checkSizeOverTime);
			ShowFloatValue(data.sizeOverTime, data.checkSizeOverTime, "Size/Time", 0.25f, -1.0f, 1.0f);

			ImGui::DragInt("Emition Rate", &data.rateOverTime, 1.0f, 1.0f, 300.0f, "%.2f");

			ImGui::Separator();
			if (ImGui::Checkbox("Loop", &data.loop)) loopTimer.Start();

			if (!data.loop) ImGui::DragFloat("Duration", &data.duration, 0.5f, 0.5f, 20.0f, "%.2f");

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Emitter Shape"))
		{
			if (ImGui::RadioButton("Cube", data.shapeType == Shape::CUBE)) data.shapeType = Shape::CUBE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Sphere", data.shapeType == Shape::SPHERE)) data.shapeType = Shape::SPHERE;
			ImGui::SameLine();
			if (ImGui::RadioButton("Cone", data.shapeType == Shape::CONE)) data.shapeType = Shape::CONE;

			float3 pos;
			switch (data.shapeType)
			{
			case Shape::CUBE:
			{
				ImGui::Text("Box");
				pos = data.cubeCreation.Size();
				ImGui::DragFloat3("Box Size", &pos.x, 0.1f, 0.1f, 20.0f, "%.2f");

				data.cubeCreation.SetFromCenterAndSize(data.cubeCreation.CenterPoint(), pos);
				break;
			}
			case Shape::SPHERE:
			{
				ImGui::Text("Sphere");

				ImGui::Text("Particle emision from:");

				bool random = data.sType == EmitterData::EmitterSphere::RANDOM;
				bool center = data.sType == EmitterData::EmitterSphere::CENTER;
				bool border = data.sType == EmitterData::EmitterSphere::BORDER;

				if (ImGui::RadioButton("Random", random))
					data.sType = EmitterData::EmitterSphere::RANDOM;
				ImGui::SameLine();
				if (ImGui::RadioButton("Center", center))
					data.sType = EmitterData::EmitterSphere::CENTER;
				ImGui::SameLine();
				if (ImGui::RadioButton("Border", border))
					data.sType = EmitterData::EmitterSphere::BORDER;

				ImGui::DragFloat("Sphere Size", &data.sphereCreation.r, 0.25f, 1.0f, 20.0f, "%.2f");
				break;
			}
			case Shape::CONE:
			{
				ImGui::Text("Cone");
				ImGui::DragFloat("End Radious", &data.circleCreation.r, 0.25f, 0.25f, 20.0f, "%.2f");
				break;
			}
			default:
				break;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Particle Color"))
		{
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted("Click color square to change it");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			std::vector<FadeColor> deleteColor;
			uint posList = 0u;
			int nextPos = 100;
			Color nextColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
			int i = 0;
			for (std::vector<FadeColor>::iterator it = data.color.begin(); it != data.color.end(); ++it)
			{
				ImGui::PushID(i);
				if ((it) == data.color.begin())
				{
					if (!EditColor(*it)) break;
				}
				else
				{
					if (!EditColor(*it, posList)) data.color.erase(it++);
				}
				ImGui::PopID();
				++i;
				++posList;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Particle Burst"))
		{
			ImGui::Checkbox("Burst", &data.burst);
			ImGui::DragInt("Min particles", &data.minPart, 1.0f, 0, 100);
			if (data.minPart > data.maxPart) data.maxPart = data.minPart;
			ImGui::DragInt("Max Particles", &data.maxPart, 1.0f, 0, 100);
			if (data.maxPart < data.minPart) data.minPart = data.maxPart;
			ImGui::DragFloat("Repeat Time", &data.repeatTime, 0.5f, 0.0f, 0.0f, "%.1f");

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Particle Material"))
		{
			if (data.texture != nullptr)
			{
				if (data.texture->data != nullptr)
				{
					ImGui::Text("UID: "); ImGui::SameLine();
					ImGui::Button(std::to_string(data.texture->GetUID()).c_str(), ImVec2(ImGui::CalcItemWidth(), 20));
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag material from Resources Panel here to change it.");
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIALS"))
						{
							IM_ASSERT(payload->DataSize == sizeof(int));
							int payloadUID = *(const int*)payload->Data;
							data.texture = (ResourceMaterial*)App->resources->RequestResource((UID)payloadUID);
							data.texture->GenerateBuffers();
						}
						ImGui::EndDragDropTarget();
					}

					IMGUI_PRINT(IMGUI_YELLOW, "Diffuse Texture:", "%s", data.texture->name.c_str());
					IMGUI_PRINT(IMGUI_YELLOW, "Path:", "%s", data.texture->path.c_str());
					IMGUI_PRINT(IMGUI_YELLOW, "Width:", "%d", data.texture->width); ImGui::SameLine(); IMGUI_PRINT(IMGUI_YELLOW, "Height:", "%d", data.texture->height);
					ImGui::Text("Image:");
					ImGui::Image((ImTextureID)data.texture->texId, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

					if (ImGui::Button("Remove material", ImVec2(ImGui::CalcItemWidth(), 20)))
					{
						data.texture->referenceCount--;
						data.texture = nullptr;
					}
				}
			}
			else
			{
				ImGui::Text("UID: "); ImGui::SameLine();
				ImGui::Button("- None -", ImVec2(ImGui::CalcItemWidth(), 20));
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag material from Resources Panel here to change it.");
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIALS"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int payloadUID = *(const int*)payload->Data;
						data.texture = (ResourceMaterial*)App->resources->RequestResource((UID)payloadUID);
						data.texture->GenerateBuffers();
					}
					ImGui::EndDragDropTarget();
				}
			}
			ImGui::TreePop();
		}
		ImGui::Separator();

		ImGui::Checkbox("SubEmitter", &data.subEmitterActive);
		if (data.subEmitterActive)
		{
			ImGui::Separator();
			if (ImGui::TreeNodeEx("Sub Emitter"))
			{
				if (ImGui::TreeNodeEx("Sub Values"))
				{
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Active checkBox if you want a random number");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}

					ImGui::Checkbox("##Sub Speed", &data.subCheckSpeed);
					ShowFloatValue(data.subSpeed, data.subCheckSpeed, "Speed", 0.25f, 0.25f, 20.0f);

					ImGui::Checkbox("##Sub Acceleration", &data.subCheckAcceleration);
					ShowFloatValue(data.subAcceleration, data.subCheckAcceleration, "Acceleration", 0.25f, -5.0f, 5.0f);

					ImGui::Checkbox("##Sub Rotation", &data.subCheckRotation);
					ShowFloatValue(data.subRotation, data.subCheckRotation, "Initial Rotation", 0.25f, -360.0f, 360.0f);

					ImGui::Checkbox("##Sub AngVelocity", &data.subCheckAngularVelocity);
					ShowFloatValue(data.subAngularVelocity, data.subCheckAngularVelocity, "Angular Vel.", 0.25f, -45.0f, 45.0f);

					ImGui::Checkbox("##Sub AngAcceleration", &data.subCheckAngularAcceleration);
					ShowFloatValue(data.subAngularAcceleration, data.subCheckAngularAcceleration, "Angular Accel.", 0.25f, -45.0f, 45.0f);

					ImGui::Checkbox("##Sub Lifetime", &data.subCheckLife);
					ShowFloatValue(data.subParticleLife, data.subCheckLife, "Life", 0.5f, 1.0f, 20.0f);

					ImGui::Checkbox("##Sub Size", &data.subCheckSize);
					ShowFloatValue(data.subSize, data.subCheckSize, "Size", 0.1f, 0.1f, 5.0f);

					ImGui::Checkbox("##Sub SizeOverTime", &data.subCheckSizeOverTime);
					ShowFloatValue(data.subSizeOverTime, data.subCheckSizeOverTime, "Size/Time", 0.25f, -1.0f, 1.0f);

					ImGui::DragInt("Emition Quant.", &data.subRateParticles, 1.0f, 0.0f, 300.0f, "%.2f");

					ImGui::Separator();
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Sub Shape"))
				{
					if (ImGui::RadioButton("S.Cube", data.subShapeType == Shape::CUBE)) data.subShapeType = Shape::CUBE;
					ImGui::SameLine();
					if (ImGui::RadioButton("S.Sphere", data.subShapeType == Shape::SPHERE)) data.subShapeType = Shape::SPHERE;
					ImGui::SameLine();
					if (ImGui::RadioButton("S.Cone", data.subShapeType == Shape::CONE)) data.subShapeType = Shape::CONE;

					float3 pos;
					switch (data.shapeType)
					{
					case Shape::CUBE:
					{
						ImGui::Text("Sub Box");
						pos = data.subCubeCreation.Size();
						ImGui::DragFloat3("S.Box Size", &pos.x, 0.1f, 0.1f, 20.0f, "%.2f");

						data.subCubeCreation.SetFromCenterAndSize(data.subCubeCreation.CenterPoint(), pos);
						break;
					}
					case Shape::SPHERE:
					{
						ImGui::Text("Sub Sphere");
						ImGui::Text("Emision from:");

						bool random = data.subSphereType == EmitterData::EmitterSphere::RANDOM;
						bool center = data.subSphereType == EmitterData::EmitterSphere::CENTER;
						bool border = data.subSphereType == EmitterData::EmitterSphere::BORDER;

						if (ImGui::RadioButton("S.Random", random))
							data.subSphereType = EmitterData::EmitterSphere::RANDOM;
						ImGui::SameLine();
						if (ImGui::RadioButton("S.Center", center))
							data.subSphereType = EmitterData::EmitterSphere::CENTER;
						ImGui::SameLine();
						if (ImGui::RadioButton("S.Border", border))
							data.subSphereType = EmitterData::EmitterSphere::BORDER;

						ImGui::DragFloat("Sub Sphere Size", &data.subSphereCreation.r, 0.25f, 1.0f, 20.0f, "%.2f");
						break;
					}
					case Shape::CONE:
					{
						ImGui::Text("Sub Cone");
						ImGui::DragFloat("Sub End Radious", &data.subCircleCreation.r, 0.25f, 0.25f, 20.0f, "%.2f");
						break;
					}
					default:
						break;
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Sub Particle Color"))
				{
					ImGui::TextDisabled("(?)");
					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted("Click color square to change it");
						ImGui::PopTextWrapPos();
						ImGui::EndTooltip();
					}

					std::vector<FadeColor> deleteColor;
					uint posList = 0u;
					int nextPos = 100;
					Color nextColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
					int i = 0;
					for (std::vector<FadeColor>::iterator it = data.subColor.begin(); it != data.subColor.end(); ++it)
					{
						ImGui::PushID(i);
						if ((it) == data.subColor.begin())
						{
							if (!EditColor(*it)) break;
						}
						else
						{
							if (!EditColor(*it, posList)) data.subColor.erase(it++);
						}
						ImGui::PopID();
						++i;
						++posList;
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Sub Particle Material"))
				{
					if (data.subTexture != nullptr)
					{
						if (data.subTexture->data != nullptr)
						{
							ImGui::Text("UID: "); ImGui::SameLine();
							ImGui::Button(std::to_string(data.subTexture->GetUID()).c_str(), ImVec2(ImGui::CalcItemWidth(), 20));
							if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag material from Resources Panel here to change it.");
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIALS"))
								{
									IM_ASSERT(payload->DataSize == sizeof(int));
									int payloadUID = *(const int*)payload->Data;
									data.subTexture = (ResourceMaterial*)App->resources->RequestResource((UID)payloadUID);
									data.subTexture->GenerateBuffers();
								}
								ImGui::EndDragDropTarget();
							}

							IMGUI_PRINT(IMGUI_YELLOW, "Diffuse Texture:", "%s", data.subTexture->name.c_str());
							IMGUI_PRINT(IMGUI_YELLOW, "Path:", "%s", data.subTexture->path.c_str());
							IMGUI_PRINT(IMGUI_YELLOW, "Width:", "%d", data.subTexture->width); ImGui::SameLine(); IMGUI_PRINT(IMGUI_YELLOW, "Height:", "%d", data.texture->height);
							ImGui::Text("Image:");
							ImGui::Image((ImTextureID)data.subTexture->texId, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));

							if (ImGui::Button("Remove material", ImVec2(ImGui::CalcItemWidth(), 20)))
							{
								data.subTexture->referenceCount--;
								data.subTexture = nullptr;
							}
						}
					}
					else
					{
						ImGui::Text("UID: "); ImGui::SameLine();
						ImGui::Button("- None -", ImVec2(ImGui::CalcItemWidth(), 20));
						if (ImGui::IsItemHovered()) ImGui::SetTooltip("Drag material from Resources Panel here to change it.");
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MATERIALS"))
							{
								IM_ASSERT(payload->DataSize == sizeof(int));
								int payloadUID = *(const int*)payload->Data;
								data.subTexture = (ResourceMaterial*)App->resources->RequestResource((UID)payloadUID);
								data.subTexture->GenerateBuffers();
							}
							ImGui::EndDragDropTarget();
						}
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		ImGui::Separator();

		if (ImGui::Button("Remove Particles", ImVec2(150, 25))) ClearEmitter();
	}
}

void ComponentEmitter::ClearEmitter()
{
	for (std::vector<Particle*>::iterator it = particlePool.begin(); it != particlePool.end(); ++it)
	{
		if ((*it) != nullptr && (*it)->active)
		{
			delete (*it);
			LOG("Particle Destroyed");
		}
	}
	particlePool.clear();

	data.color.clear();
	data.texture = nullptr;
}

float ComponentEmitter::GenerateRandBetween(float min, float max)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = max - min;
	float result = random * diff;

	return min + result;
}

void ComponentEmitter::ShowFloatValue(float2& value, bool checkBox, const char* name, float v_speed, float v_min, float v_max)
{
	ImGui::SameLine();
	if (checkBox)
	{
		ImGui::PushItemWidth(70.0f);
		std::string str = "##";
		str.append(name);
		str.append("min");
		if (ImGui::DragFloat(str.data(), &value.x, v_speed, v_min, v_max, "%.2f"))
		{
			if (value.x > value.y) value.y = value.x;
		}
		ImGui::SameLine();
		if (ImGui::DragFloat(name, &value.y, v_speed, v_min, v_max, "%.2f"))
		{
			if (value.x > value.y) value.y = value.x;
		}
	}
	else
	{
		ImGui::PushItemWidth(148.0f);
		if (ImGui::DragFloat(name, &value.x, v_speed, v_min, v_max, "%.2f")) value.y = value.x;
	}
	ImGui::PopItemWidth();
}

void ComponentEmitter::CreateParticles(int num, const float3& pos, bool sub)
{
	for (int i = 0; i < num; ++i)
	{
		float3 initialPos = pos;
		float3 dir = float3::zero;
		ResourceMaterial* tex;
		float life;
		float3 scale;
		float pAngle;
		float acc;
		float vel;
		float incrementSize;
		float angularAcc;
		float angularVel;
		std::vector<FadeColor> color;

		if (!sub)
		{
			ShapeParticleData(initialPos, dir);

			tex = data.texture;
			life = GenerateRandBetween(data.particleLife.x, data.particleLife.y);
			scale = float3::one * GenerateRandBetween(data.size.x, data.size.y);
			pAngle = GenerateRandBetween(data.rotation.x, data.rotation.y);
			acc = GenerateRandBetween(data.acceleration.x, data.acceleration.y);
			vel = GenerateRandBetween(data.speed.x, data.speed.y);
			incrementSize = GenerateRandBetween(data.sizeOverTime.x, data.sizeOverTime.y);
			angularAcc = GenerateRandBetween(data.angularAcceleration.x, data.angularAcceleration.y);
			angularVel = GenerateRandBetween(data.angularVelocity.x, data.angularVelocity.y);
			color = data.color;
		}
		else
		{
			ShapeParticleData(initialPos, dir, true);

			tex = data.subTexture;
			life = GenerateRandBetween(data.subParticleLife.x, data.subParticleLife.y);
			scale = float3::one * GenerateRandBetween(data.subSize.x, data.subSize.y);
			pAngle = GenerateRandBetween(data.subRotation.x, data.subRotation.y);
			acc = GenerateRandBetween(data.subAcceleration.x, data.subAcceleration.y);
			vel = GenerateRandBetween(data.subSpeed.x, data.subSpeed.y);
			incrementSize = GenerateRandBetween(data.subSizeOverTime.x, data.subSizeOverTime.y);
			angularAcc = GenerateRandBetween(data.subAngularAcceleration.x, data.subAngularAcceleration.y);
			angularVel = GenerateRandBetween(data.subAngularVelocity.x, data.subAngularVelocity.y);
			color = data.subColor;
		}

		Particle* p = new Particle(data.plane, tex, life, initialPos, scale, pAngle, acc, vel, dir, incrementSize, angularAcc, angularVel, color);
		p->owner = this;
		particlePool.push_back(p);
	}
}

void ComponentEmitter::ShapeParticleData(float3& pos, float3& dir, bool sub)
{
	float3 spawn = float3::zero;
	Shape s = Shape::NONE;
	AABB cube;
	Sphere sphere;
	EmitterData::EmitterSphere sType;
	Circle circle;
	if (!sub)
	{
		s = data.shapeType;
		cube = data.cubeCreation;
		sphere = data.sphereCreation;
		sType = data.sType;
		circle = data.circleCreation;
	}
	else
	{
		s = data.subShapeType;
		cube = data.subCubeCreation;
		sphere = data.subSphereCreation;
		sType = data.subSphereType;
		circle = data.subCircleCreation;
	}

	switch (s)
	{
	case Shape::CUBE:
	{
		spawn = cube.RandomPointInside(LCG());
		dir = (float3::unitY * owner->GetComponent<ComponentTransform>()->GetRot().ToFloat3x3()).Normalized();
		break;
	}
	case Shape::SPHERE:
	{
		switch (sType)
		{
		case EmitterData::EmitterSphere::RANDOM:
		{
			spawn = sphere.RandomPointInside(LCG());
			dir = spawn.Normalized();
			break;
		}
		case EmitterData::EmitterSphere::CENTER:
		{
			dir = sphere.RandomPointInside(LCG()).Normalized();
			break;
		}
		case EmitterData::EmitterSphere::BORDER:
		{
			spawn = sphere.RandomPointOnSurface(LCG());
			dir = spawn.Normalized();
			break;
		}
		}
		break;
	}
	case Shape::CONE:
	{
		float angle = (2 * pi) * LCG().Int() / MAXUINT;
		float centerDist = (float)LCG().Int() / MAXUINT;

		circle.pos = (float3::unitY * owner->GetComponent<ComponentTransform>()->GetRot().ToFloat3x3()).Normalized();
		circle.normal = -circle.pos;
		dir = (circle.GetPoint(angle, centerDist)).Normalized();
		break;
	}
	default:
		break;
	}

	pos += spawn + owner->GetComponent<ComponentTransform>()->GetGlobalPos();

}

void ComponentEmitter::DestroyParticle(Particle* p)
{
	int i = -1;
	std::vector<Particle*>::iterator it = particlePool.begin();
	while (it != particlePool.end())
	{
		if ((*it) == p)
		{
			delete p;
			break;
		}
		it++;
	}
}

bool ComponentEmitter::EditColor(FadeColor& color, uint pos)
{
	bool ret = true;
	ImVec4 vecColor = ImVec4(color.color.x, color.color.y, color.color.z, color.color.w);
	if (ImGui::ColorButton("Color", vecColor, ImGuiColorEditFlags_None, ImVec2(100, 20)))
	{
		color.changingColor = !color.changingColor;
	}

	if (!color.changingColor)
	{
		ImGui::SameLine();
		ImGui::TextUnformatted("Color");
		if (pos > 0)
		{
			std::string colorStr = "Remove Color ";
			colorStr.append(std::to_string(pos));
			ImGui::SameLine();
			if (ImGui::Button(colorStr.data(), ImVec2(125, 25))) ret = false;
		}
		else if (!data.timeColor)
		{
			ret = false;
		}
	}
	else
	{
		ImGui::ColorEdit4("Color", &color.color.x, ImGuiColorEditFlags_AlphaBar);
	}
	return ret;
}