#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleGameObjects.h"
#include "ModuleScene.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	right = float3(1.0f, 0.0f, 0.0f);
	up = float3(0.0f, 1.0f, 0.0f);
	front = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 5.0f, -15.0f);
	reference = float3(0.0f, 0.0f, 0.0f);

	cameraFrustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);

	CalculateViewMatrix();
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera\n");

	LookAt(float3::zero);

	bool ret = true;

	aspectRatio = 1.f;
	verticalFOV = 60.f;
	nearPlaneDistance = 0.1f;
	farPlaneDistance = 1000.f;
	cameraSensitivity = .1f;
	cameraSpeed = 60.f;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
UpdateStatus ModuleCamera3D::Update(float dt)
{
	if (!App->gui->MouseOnScene()) return UpdateStatus::UPDATE_CONTINUE;

	float3 newPos(0, 0, 0);
	float speed = cameraSpeed * dt;

	//Focus
	if (App->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN)
	{
		if (App->gameObjects->selectedGameObject != nullptr)
		{
			if (ComponentMesh* mesh = App->gameObjects->selectedGameObject->GetComponent<Mesh>())
			{
				const float3 meshCenter = mesh->GetCenterPointInWorldCoords();
				LookAt(meshCenter);
				const float meshRadius = mesh->radius;
				const float currentDistance = meshCenter.Distance(position);
				const float desiredDistance = (meshRadius * 2) / atan(cameraFrustum.HorizontalFov());
				position = position + front * (currentDistance - desiredDistance);
			}
			else
			{
				LookAt(App->gameObjects->selectedGameObject->GetComponent<Transform>()->GetPos());
			}
		}
	}

	bool hasRotated = false;

	if (App->input->GetMouseZ() > 0) newPos += front * speed * 2;
	if (App->input->GetMouseZ() < 0) newPos -= front * speed * 2;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speed *= 4.f;

		if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT) newPos += front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT) newPos -= front * speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT) newPos += right * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT) newPos -= right * speed;

		// Mouse Motion ---------------------------------------
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		{
			if (App->gameObjects->selectedGameObject != nullptr)
			{
				const float newDeltaX = (float)dx * cameraSensitivity;
				const float newDeltaY = (float)dy * cameraSensitivity;

				Quat yRotation(up, dx * dt * cameraSensitivity * 0.7f);
				Quat xRotation(right, dy * dt * cameraSensitivity * 0.7f);

				float3 distance = position - reference;
				distance = xRotation.Transform(distance);
				distance = yRotation.Transform(distance);

				position = distance + reference;

				CalculateViewMatrix();
				LookAt(reference);
			}
		}
		else
		{
			if (dx != 0)
			{
				const float newDeltaX = (float)dx * cameraSensitivity;
				float deltaX = newDeltaX + 0.65f * (lastDeltaX - newDeltaX); //lerp for smooth rotation acceleration to avoid jittering
				lastDeltaX = deltaX;
				Quat rotateY = Quat::RotateY(up.y >= 0.f ? deltaX * .1f : -deltaX * .1f);
				up = rotateY * up;
				front = rotateY * front;
				CalculateViewMatrix();
				hasRotated = true;
			}

			if (dy != 0)
			{
				const float newDeltaY = (float)dy * cameraSensitivity;
				float deltaY = newDeltaY + 0.65f * (lastDeltaY - newDeltaY); //lerp for smooth rotation acceleration to avoid jittering
				lastDeltaY = deltaY;
				Quat rotateX = Quat::RotateAxisAngle(right, -deltaY * .1f);
				up = rotateX * up;
				front = rotateX * front;
				CalculateViewMatrix();
				hasRotated = true;
			}
		}
	}

	position += newPos;

	!hasRotated ? lastDeltaX = lastDeltaY = 0.f : 0.f;

	//Mouse Picking
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_DOWN && !ImGuizmo::IsOver() && !ImGuizmo::IsUsing())
	{
		GameObject* picked = MousePicking();
		App->gameObjects->selectedGameObject = picked;
	}

	return UpdateStatus::UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& point)
{
	reference = point;

	front = (reference - position).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	if (projectionIsDirty) RecalculateProjection();

	cameraFrustum.SetPos(position);
	cameraFrustum.SetFront(front.Normalized());
	cameraFrustum.SetUp(up.Normalized());
	float3::Orthonormalize((float3)cameraFrustum.Front(), (float3)cameraFrustum.Up());
	right = up.Cross(front);
	viewMatrix = cameraFrustum.ViewMatrix();
}

void ModuleCamera3D::RecalculateProjection()
{
	cameraFrustum.SetViewPlaneDistances(nearPlaneDistance, farPlaneDistance);
	cameraFrustum.SetVerticalFovAndAspectRatio(verticalFOV * DEG_TO_RAD, aspectRatio);
}

GameObject* ModuleCamera3D::MousePicking()
{
	float normalX = App->gui->mouseScenePosition.x / App->gui->viewportSize.x;
	float normalY = App->gui->mouseScenePosition.y / App->gui->viewportSize.y;

	normalX = (normalX - 0.5f) * 2.0f;
	normalY = -(normalY - 0.5f) * 2.0f;

	LineSegment newRay = cameraFrustum.UnProjectLineSegment(normalX, normalY);
	App->scene->ray = newRay;

	std::vector<GameObject*> sceneGameObjects = App->gameObjects->GetAllGameObjects();
	std::map<float, GameObject*> hitGameObjects;

	//Find all hit GameObjects
	for (size_t i = 0; i < sceneGameObjects.size(); i++)
	{
		ComponentMesh* m = sceneGameObjects[i]->GetComponent<Mesh>();
		if (m != nullptr)
		{
			bool hit = newRay.Intersects(m->bbox);

			if (hit)
			{
				float dNear;
				float dFar;
				hit = newRay.Intersects(m->bbox, dNear, dFar);
				hitGameObjects[dNear] = sceneGameObjects[i];
			}
		}
		else if (m == nullptr)
		{
			ComponentCamera* c = sceneGameObjects[i]->GetComponent<Camera>();
			if (c != nullptr)
			{
				bool hit = newRay.Intersects(c->bbox);

				if (hit)
				{
					float dNear;
					float dFar;
					hit = newRay.Intersects(c->bbox, dNear, dFar);
					hitGameObjects[dNear] = sceneGameObjects[i];
				}
			}
			else if (c == nullptr)
			{
				ComponentEmitter* e = sceneGameObjects[i]->GetComponent<Emitter>();
				if (e != nullptr)
				{
					bool hit = newRay.Intersects(e->bbox);

					if (hit)
					{
						float dNear;
						float dFar;
						hit = newRay.Intersects(e->bbox, dNear, dFar);
						hitGameObjects[dNear] = sceneGameObjects[i];
					}
				}
			}
		}
	}

	for (std::map<float, GameObject*>::iterator it = hitGameObjects.begin(); it != hitGameObjects.end(); it++)
	{
		GameObject* gameObject = it->second;

		LineSegment rayLocal = newRay;
		rayLocal.Transform(gameObject->GetComponent<Transform>()->GetGlobalTransform().Inverted());

		ComponentMesh* cMesh = gameObject->GetComponent<Mesh>();

		if (cMesh != nullptr)
		{
			ResourceMesh* rMesh = cMesh->mesh;

			if (rMesh == nullptr) continue;

			for (size_t i = 0; i < rMesh->indexNum; i += 3)
			{
				//create every triangle
				float3 v1;
				v1.x = rMesh->vertices[rMesh->indices[i]].x;
				v1.y = rMesh->vertices[rMesh->indices[i]].y;
				v1.z = rMesh->vertices[rMesh->indices[i]].z;

				float3 v2;
				v2.x = rMesh->vertices[rMesh->indices[i + 1]].x;
				v2.y = rMesh->vertices[rMesh->indices[i + 1]].y;
				v2.z = rMesh->vertices[rMesh->indices[i + 1]].z;

				float3 v3;
				v3.x = rMesh->vertices[rMesh->indices[i + 2]].x;
				v3.y = rMesh->vertices[rMesh->indices[i + 2]].y;
				v3.z = rMesh->vertices[rMesh->indices[i + 2]].z;

				const Triangle triangle(v1, v2, v3);

				float distance;
				float3 intersectionPoint;
				if (rayLocal.Intersects(triangle, &distance, &intersectionPoint)) return gameObject;
			}
		}
		else
		{
			ComponentCamera* cam = gameObject->GetComponent<Camera>();
			if (cam != nullptr)
			{
				return gameObject;
			}
			else
			{
				ComponentEmitter* e = gameObject->GetComponent<Emitter>();
				if (e != nullptr)
				{
					return gameObject;
				}
			}
		}
	}

	return nullptr;
}

void ModuleCamera3D::OnSave(JSONWriter& writer) const
{
	writer.String("camera");
	writer.StartObject();
	SAVE_JSON_FLOAT(cameraSpeed);
	SAVE_JSON_FLOAT(cameraSensitivity);
	SAVE_JSON_FLOAT(verticalFOV);
	SAVE_JSON_FLOAT(nearPlaneDistance);
	SAVE_JSON_FLOAT(farPlaneDistance);
	writer.EndObject();
}

void ModuleCamera3D::OnLoad(const JSONReader& reader)
{
	if (reader.HasMember("camera"))
	{
		const auto& config = reader["camera"];
		LOAD_JSON_FLOAT(cameraSpeed);
		LOAD_JSON_FLOAT(cameraSensitivity);
		LOAD_JSON_FLOAT(verticalFOV);
		LOAD_JSON_FLOAT(nearPlaneDistance);
		LOAD_JSON_FLOAT(farPlaneDistance);
	}
	RecalculateProjection();
}