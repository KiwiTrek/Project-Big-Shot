#include "RenderGlobals.h"
#include "Gameobject.h"

#include "Application.h"
#include "ModuleGameObjects.h"
#include "ModuleRenderer3D.h"

ComponentBillboard::ComponentBillboard(bool active) : Component(type, active)
{
    type = ComponentTypes::BILLBOARD;
}

ComponentBillboard::~ComponentBillboard()
{}

void ComponentBillboard::Update(float dt, Application* App)
{
	if (owner->parent && bType != BillboardType::NONE)
	{
		float3 x, y, z;
		float3 obPos = owner->GetComponent<Transform>()->GetGlobalPos();
		GameObject* camera = App->gameObjects->mainCamera;

		switch (bType)
		{
		case BillboardType::NONE:
		{
			break;
		}
		case BillboardType::SCREEN:
		{
			z = -camera->GetComponent<Camera>()->frustum.Front();
			y = camera->GetComponent<Camera>()->frustum.Up();
			x = y.Cross(z).Normalized();

			break;
		}
		case BillboardType::WORLD:
		{
			z = float3(camera->GetComponent<Transform>()->GetPos() - obPos).Normalized();
			y = camera->GetComponent<Camera>()->frustum.Up();
			x = y.Cross(z).Normalized();

			y = z.Cross(x).Normalized();
			break;
		}
		case BillboardType::AXIALY:
		{
			if (xRot)
			{
				z = float3(camera->GetComponent<Transform>()->GetPos() - obPos).Normalized();
				x = owner->GetComponent<Transform>()->GetGlobalRot() * float3(1, 0, 0);
				y = z.Cross(x).Normalized();

				z = x.Cross(y).Normalized();

			}
			else if (yRot)
			{
				z = float3(camera->GetComponent<Transform>()->GetPos() - obPos).Normalized();
				y = owner->GetComponent<Transform>()->GetGlobalRot() * float3(0, 1, 0);
				x = y.Cross(z).Normalized();

				z = x.Cross(y).Normalized();

			}

			else if (zRot)
			{
				x = float3(camera->GetComponent<Transform>()->GetPos() - obPos).Normalized();
				z = owner->GetComponent<Transform>()->GetGlobalRot() * float3(0,0,1);
				y = z.Cross(x).Normalized();

				x = y.Cross(z).Normalized();
			}
			break;
		}
		default:
			break;
		}


		LineSegment ray(obPos, x * 10);
		App->renderer3D->DrawRay(ray);

		LineSegment ray2(obPos, y * 10);
		App->renderer3D->DrawRay(ray2);

		LineSegment ray3(obPos, z * 10);
		App->renderer3D->DrawRay(ray3);

		float3x3 rot = float3x3(x, y, z);

		float4x4 newOrient = float4x4::FromTRS(owner->GetComponent<Transform>()->GetGlobalPos(), rot, owner->GetComponent<Transform>()->GetGlobalScale());

		float4x4 temp = owner->parent->GetComponent<Transform>()->GetGlobalTransform();
		temp.Inverse();

		newOrient = temp.Mul(newOrient);
		if (owner->GetComponent<Transform>()) owner->GetComponent<Transform>()->SetGlobalTransform(newOrient);

	}
}

void ComponentBillboard::DrawInspector(Application* App)
{
	if (ImGui::CollapsingHeader("Billboard", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Checkbox("Enable Billboard", &active);

		ImGui::Text("Current billboard:");
		ImGui::SameLine();
		switch (bType)
		{
		case BillboardType::NONE:
			ImGui::Text("NONE");
			break;
		case BillboardType::SCREEN:
			ImGui::Text("Screen");
			break;
		case BillboardType::WORLD:
			ImGui::Text("World");
			break;
		case BillboardType::AXIALY:
			ImGui::Text("Axialy");
			break;
		default:
			break;
		}

		if (bType == BillboardType::AXIALY)
		{
			if (ImGui::RadioButton("X", xRot))
			{
				xRot = true;
				yRot = false;
				zRot = false;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Y", yRot))
			{
				xRot = false;
				yRot = true;
				zRot = false;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("Z", zRot))
			{
				xRot = false;
				yRot = false;
				zRot = true;
			}
		}
		if (ImGui::BeginMenu("Change your billboard"))
		{
			if (ImGui::MenuItem("Screen"))
			{
				bType = BillboardType::SCREEN;
			}
			else if (ImGui::MenuItem("World"))
			{
				bType = BillboardType::WORLD;
			}
			else if (ImGui::MenuItem("Axialy"))
			{
				bType = BillboardType::AXIALY;
			}
			else if (ImGui::MenuItem("NONE"))
			{
				bType = BillboardType::NONE;
			}
			ImGui::End();
		}
	}
}
