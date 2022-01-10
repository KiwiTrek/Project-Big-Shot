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

void ComponentEmitter::Update(float dt, Application* App)
{
	bbox.SetFromCenterAndSize(owner->GetComponent<ComponentTransform>()->GetPos(), vec(1.1f, 1.1f, 1.1f));

	float3 defaultPos = float3(0.5, 1.0, -0.5);
	float time = lifeTimer.ReadSec();
	if (time > data.timeToParticle && (data.loop || loopTimer.ReadSec() < data.duration))
	{
		int particlesToCreate = (time / (1.0f / data.rateOverTime));
		CreateParticles(particlesToCreate, defaultPos);
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
		CreateParticles(particlesToCreate, defaultPos);
		burstTimer.Start();
	}

	if (!newPositions.empty())
	{
		for (std::vector<float3>::iterator it = newPositions.begin(); it != newPositions.end(); ++it)
		{
			CreateParticles(data.subRateOverTime, (*it), true);
		}
		newPositions.clear();
	}

	// Update all alive particles
	for (std::vector<Particle*>::iterator p = particlePool.begin(); p != particlePool.end(); ++p)
	{
		if ((*p)->active == false)
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

			ImGui::DragInt("Emition", &data.rateOverTime, 1.0f, 0.0f, 300.0f, "%.2f");

			ImGui::Separator();
			if (ImGui::Checkbox("Loop", &data.loop)) loopTimer.Start();

			if (!data.loop) ImGui::DragFloat("Duration", &data.duration, 0.5f, 0.5f, 20.0f, "%.2f");

			ImGui::TreePop();
		}

		ImGui::Separator();

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

		ImGui::Separator();

		if (ImGui::TreeNodeEx("Particle Color"))
		{
			ImGui::Text("Particle Color");
			ImGui::SameLine();

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
				//TODO: they must be able to change position
				ImGui::PushID(i);
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
				ImGui::PopID();
				++i;
				++posList;
			}


			//ImGui::Checkbox("Color time", &data.timeColor);
			//if (data.timeColor)
			//{
			//	ImGui::DragInt("Position", &nextPos, 1.0f, 1, 100);
			//	ImGui::ColorPicker4("", &nextColor.x, ImGuiColorEditFlags_AlphaBar);
			//	if (ImGui::Button("Add Color", ImVec2(125, 25)))
			//	{
			//		FadeColor newColor = FadeColor(nextColor,nextPos/100,false);
			//		data.color.push_back(newColor);
			//		//sort the colors by pos
			//		//data.color.sort();
			//	}
			//}


			ImGui::TreePop();
		}

		ImGui::Separator();

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

		ImGui::Separator();

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

		/*
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
		*/

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

void ComponentEmitter::CreateParticles(int num, const float3& pos, bool sub)
{
	for (int i = 0; i < num; ++i)
	{
		float3 spawnPos = pos;
		float3 spawn = float3::zero;
		float angle = 0.0f;
		float centerDist = 0.0f;

		switch (data.shapeType)
		{
		case Shape::CUBE:
			spawn = data.cubeCreation.RandomPointInside(LCG());
			data.particleDirection = (float3::unitY * owner->GetComponent<ComponentTransform>()->GetRot().ToFloat3x3()).Normalized();
			break;

		case Shape::SPHERE:
			switch (data.sType)
			{
			case EmitterData::EmitterSphere::RANDOM:
				spawn = data.sphereCreation.RandomPointInside(LCG());
				data.particleDirection = spawn.Normalized();
				break;
			case EmitterData::EmitterSphere::CENTER:
				data.particleDirection = data.sphereCreation.RandomPointInside(LCG()).Normalized();
				break;
			case EmitterData::EmitterSphere::BORDER:
				spawn = data.sphereCreation.RandomPointOnSurface(LCG());
				data.particleDirection = spawn.Normalized();
				break;
			}
			break;

		case Shape::CONE:

			angle = (2 * pi) * LCG().Int() / MAXUINT;
			centerDist = (float)LCG().Int() / MAXUINT;

			data.circleCreation.pos = (float3::unitY * owner->GetComponent<ComponentTransform>()->GetRot().ToFloat3x3()).Normalized();
			data.circleCreation.normal = -data.circleCreation.pos;
			data.particleDirection = (data.circleCreation.GetPoint(angle, centerDist)).Normalized();
			break;
		default:
			break;
		}

		float3 global = owner->GetComponent<ComponentTransform>()->GetGlobalPos();

		spawnPos += spawn + global;

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
			tex = data.texture;
			life = GenerateRandNum(data.particleLife.x, data.particleLife.y);
			scale = float3::one * GenerateRandNum(data.size.x, data.size.y);
			pAngle = GenerateRandNum(data.rotation.x, data.rotation.y);
			acc = GenerateRandNum(data.acceleration.x, data.acceleration.y);
			vel = GenerateRandNum(data.speed.x, data.speed.y);
			incrementSize = GenerateRandNum(data.sizeOverTime.x, data.sizeOverTime.y);
			angularAcc = GenerateRandNum(data.angularAcceleration.x, data.angularAcceleration.y);
			angularVel = GenerateRandNum(data.angularVelocity.x, data.angularVelocity.y);
			color = data.color;
		}
		else
		{
			tex = data.subTexture;
			life = GenerateRandNum(data.subParticleLife.x, data.subParticleLife.y);
			scale = float3::one * GenerateRandNum(data.subSize.x, data.subSize.y);
			pAngle = GenerateRandNum(data.subRotation.x, data.subRotation.y);
			acc = GenerateRandNum(data.subAcceleration.x, data.subAcceleration.y);
			vel = GenerateRandNum(data.subSpeed.x, data.subSpeed.y);
			incrementSize = GenerateRandNum(data.subSizeOverTime.x, data.subSizeOverTime.y);
			angularAcc = GenerateRandNum(data.subAngularAcceleration.x, data.subAngularAcceleration.y);
			angularVel = GenerateRandNum(data.subAngularVelocity.x, data.subAngularVelocity.y);
			color = data.subColor;
		}

		Particle* p = new Particle(data.plane, tex, life, spawnPos, scale, pAngle, acc, vel, data.particleDirection, incrementSize, angularAcc, angularVel, color);
		p->owner = this;
		particlePool.push_back(p);
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
