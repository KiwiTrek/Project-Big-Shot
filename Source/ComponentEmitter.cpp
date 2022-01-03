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
}

ComponentEmitter::~ComponentEmitter()
{
	particlePool.clear();
}

void ComponentEmitter::Update(float dt, Application* App)
{
	float time = lifeTimer.ReadSec();
	if (time > data.timeToParticle && (data.loop || loopTimer.ReadSec() < data.duration))
	{
		int particlesToCreate = (time / (1.0f / data.rateOverTime));
		CreateParticles(particlesToCreate, float3::zero,App->particles->lastUsedParticle);

		data.timeToParticle = (1.0f / data.rateOverTime);
		lifeTimer.Start();
	}

	float burstTime = burstTimer.ReadSec();
	if (data.burst && burstTime > data.repeatTime)
	{
		int particlesToCreate = data.minPart;
		if (data.minPart != data.maxPart)
		{
			particlesToCreate = (rand() % (data.maxPart - data.minPart)) + data.minPart;
		}
		CreateParticles(particlesToCreate, float3::zero,App->particles->lastUsedParticle);
		burstTimer.Start();
	}

	if (data.isSubEmitter && data.subEmitterActive)
	{
		data.subEmitter->GetComponent<Emitter>()->Update(dt, App);
	}

	// Update all alive particles
	for (std::vector<Particle*>::iterator p = particlePool.begin(); p != particlePool.end(); ++p)
	{
		(*p)->Update(dt);
	}
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

			ImGui::Checkbox("##AngularVelocity", &data.checkAngularVelocity);
			ShowFloatValue(data.angularVelocity, data.checkAngularVelocity, "Angular Velocity", 0.25f, -45.0f, 45.0f);

			ImGui::Checkbox("##AngularAcceleration", &data.checkAngularAcceleration);
			ShowFloatValue(data.angularAcceleration, data.checkAngularAcceleration, "Angular Acceleration", 0.25f, -45.0f, 45.0f);

			ImGui::Checkbox("##Lifetime", &data.checkLife);
			ShowFloatValue(data.particleLife, data.checkLife, "Lifetime", 0.5f, 1.0f, 20.0f);

			ImGui::Checkbox("##Size", &data.checkSize);
			ShowFloatValue(data.size, data.checkSize, "Size", 0.1f, 0.1f, 5.0f);

			ImGui::Checkbox("##SizeOverTime", &data.checkSizeOverTime);
			ShowFloatValue(data.sizeOverTime, data.checkSizeOverTime, "SizeOverTime", 0.25f, -1.0f, 1.0f);

			ImGui::DragInt("Emition", &data.rateOverTime, 1.0f, 0.0f, 300.0f, "%.2f");

			ImGui::Separator();
			if (ImGui::Checkbox("Loop", &data.loop))
				loopTimer.Start();
			ImGui::DragFloat("Duration", &data.duration, 0.5f, 0.5f, 20.0f, "%.2f");

			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Emitter Shape"))
		{
			ImGui::Separator();
			if (ImGui::BeginMenu("Change Shape"))
			{
				if (ImGui::MenuItem("Box"))
					data.shapeType = Shape::CUBE;
				else if (ImGui::MenuItem("Sphere"))
					data.shapeType = Shape::SPHERE;
				else if (ImGui::MenuItem("Cone"))
					data.shapeType = Shape::CONE;
				ImGui::End();
			}

			float3 pos;
			switch (data.shapeType)
			{
			case Shape::CUBE:
				ImGui::Text("Box");
				pos = data.cubeCreation.Size();
				ImGui::DragFloat3("Box Size", &pos.x, 0.1f, 0.1f, 20.0f, "%.2f");

				data.cubeCreation.SetFromCenterAndSize(data.cubeCreation.CenterPoint(), pos);

				break;
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
				ImGui::Text("Cone");
				ImGui::DragFloat("Sphere Size", &data.circleCreation.r, 0.25f, 0.25f, 20.0f, "%.2f");

				break;
			default:
				break;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Particle Color"))

		{
			ImGui::Text("Particle Color");
			ImGui::SameLine();

			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted("Click color square for change it");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			std::vector<FadeColor> deleteColor;
			uint posList = 0u;
			int nextPos = 100;
			Color nextColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
			for (std::vector<FadeColor>::iterator it = data.color.begin();it != data.color.end();++it)
			{
				//TODO: they must be able to change position
				if ((it) == data.color.begin())
				{//Cant delete 1st color

					if (!EditColor(*it))
						break;
				}
				else
				{
					if (!EditColor(*it, posList))
						data.color.erase(it++);
				}
				++posList;
			}
			ImGui::Separator();
			ImGui::Checkbox("Color time", &data.timeColor);
			if (data.timeColor)
			{

				ImGui::DragInt("Position", &nextPos, 1.0f, 1, 100);
				ImGui::ColorPicker4("", &nextColor.x, ImGuiColorEditFlags_AlphaBar);
				if (ImGui::Button("Add Color", ImVec2(125, 25)))
				{
					FadeColor newColor = FadeColor(nextColor,nextPos/100,false);
					data.color.push_back(newColor);
					//sort the colors by pos
					//data.color.sort();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Particle Burst"))
		{
			std::string burstTypeName = "Smoke";
			ImGui::Checkbox("Burst", &data.burst);
			if (ImGui::BeginMenu(burstTypeName.data()))
			{
				if (ImGui::MenuItem("Smoke"))
				{
					data.eType = EmitterData::EmitterType::SMOKE;
					burstTypeName = "Smoke Burst";
				}
				else if (ImGui::MenuItem("Firework Projectile"))
				{
					data.eType = EmitterData::EmitterType::FIREWORK_PROJECTILE;
					burstTypeName = "Firework Projectile Burst";
				}
				else if (ImGui::MenuItem("Firework Explosion"))
				{
					data.eType = EmitterData::EmitterType::FIREWORK_EXPLOSION;
					burstTypeName = "Firework Explosion Burst";
				}
				ImGui::End();
			}
			ImGui::DragInt("Min particles", &data.minPart, 1.0f, 0, 100);
			if (data.minPart > data.maxPart)
				data.maxPart = data.minPart;
			ImGui::DragInt("Max Particles", &data.maxPart, 1.0f, 0, 100);
			if (data.maxPart < data.minPart)
				data.minPart = data.maxPart;
			ImGui::DragFloat("Repeat Time", &data.repeatTime, 0.5f, 0.0f, 0.0f, "%.1f");

			ImGui::Separator();
			ImGui::TreePop();
		}

		/*if (ImGui::TreeNode("Particle BoundingBox"))
		{
			ImGui::Checkbox("Bounding Box", &data.drawAABB);
			if (data.drawAABB)
			{
				float3 size = owner->transform->originalBoundingBox.Size();
				if (ImGui::DragFloat3("Dimensions", &size.x, 1.0f, 0.0f, 0.0f, "%.0f"))
				{
					owner->transform->originalBoundingBox.SetFromCenterAndSize(data.posDifAABB, size);
					owner->transform->UpdateBoundingBox();
				}

				if (ImGui::DragFloat3("Pos", &data.posDifAABB.x, 1.0f, 0.0f, 0.0f, "%.0f"))
				{
					owner->transform->originalBoundingBox.SetFromCenterAndSize(data.posDifAABB, size);
					owner->transform->UpdateBoundingBox();
				}
			}
		}*/

		if (ImGui::Checkbox("SubEmitter", &data.subEmitterActive))
		{
			if (data.subEmitterActive)
			{
				if (data.subEmitter)
				{
					data.subEmitter->active = true;
					data.subEmitter->GetComponent<Emitter>()->active = true;
				}
				else
				{
					data.subEmitter = new GameObject("SubEmition");
					data.subEmitter->parent = owner;
					EmitterData info;
					info.isSubEmitter = true;
					data.subEmitter->CreateComponent(ComponentTypes::EMITTER);
					data.subEmitter->GetComponent<Emitter>()->data = info;
					//AABB boundingBox = AABB();
					//boundingBox.SetFromCenterAndSize(data.subEmitter->GetPos(), float3::one);
					//data.subEmitter->SetABB(boundingBox);
				}
			}
			else
				data.subEmitter->active = false;
		}
		ImGui::Separator();

		if (ImGui::Button("Remove Particles", ImVec2(150, 25)))
			ClearEmitter();
			//toDelete = true;
	}
}

void ComponentEmitter::ClearEmitter()
{
	if (data.isSubEmitter || data.subEmitter || data.subEmitterActive)
	{
		data.subEmitter->GetComponent<Emitter>()->ClearEmitter();
		delete data.subEmitter;
		data.subEmitter = nullptr;
	}

	for (std::vector<Particle*>::iterator it = particlePool.begin(); it != particlePool.end(); ++it)
	{
		bool ret;
		(*it)->EndParticle(ret);
	}
	particlePool.clear();

	data.color.clear();
	data.texture = nullptr;
}

float ComponentEmitter::GenerateRandNum(float min, float max)
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
			if (value.x > value.y)
			{
				value.y = value.x;
			}
		}
		ImGui::SameLine();
		if (ImGui::DragFloat(name, &value.y, v_speed, v_min, v_max, "%.2f"))
		{
			if (value.x > value.y)
			{
				value.y = value.x;
			}
		}
	}
	else
	{
		ImGui::PushItemWidth(148.0f);
		if (ImGui::DragFloat(name, &value.x, v_speed, v_min, v_max, "%.2f"))
			value.y = value.x;
	}
	ImGui::PopItemWidth();
}

void ComponentEmitter::CreateParticles(int num, const float3& pos, int lastUsedParticle)
{
	for (int i = 0; i < num; ++i)
	{
		int id = -1;

		for (int i = lastUsedParticle; i < MAX_PARTICLES; ++i)
		{
			if (!allParticles[i].active)
			{
				lastUsedParticle = i;
				id = i;
			}
		}

		for (int i = 0; i < lastUsedParticle; ++i)
		{
			if (!allParticles[i].active)
			{
				lastUsedParticle = i;
				id = i;
			}
		}

		if (id == -1) break;

		// if we want to start the particle at a random point
		//float3 spawnPos = pos;
		//spawnPos += RandPos(shapeType);

		allParticles[id] = Particle();
		allParticles[id].owner = this;
		particlePool.push_back(&allParticles[id]);
	}
}

void ComponentEmitter::DestroyParticle(Particle* p)
{
	int i = -1;
	std::vector<Particle*>::iterator it = particlePool.begin();
	while (it != particlePool.end())
	{
		if ((*it) == p)
		{
			particlePool.erase(it);
			RELEASE(p);
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
		color.changingColor = !color.changingColor;

	if (!color.changingColor)
	{
		ImGui::SameLine();
		ImGui::TextUnformatted("Color");
		if (pos > 0)
		{
			std::string colorStr = "Remove Color ";
			colorStr.append(std::to_string(pos));
			ImGui::SameLine();
			if (ImGui::Button(colorStr.data(), ImVec2(125, 25)))
				ret = false;
		}
		else if (!data.timeColor)
			ret = false;
	}
	else
		ImGui::ColorEdit4("Color", &color.color.x, ImGuiColorEditFlags_AlphaBar);
	return ret;
}
