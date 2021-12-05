#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleGameObjects.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
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
	float3 newPos(0, 0, 0);
	float speed = cameraSpeed * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) speed *= 4.f;

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

	if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT) newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT) newPos += front * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT) newPos -= front * speed;

	if (App->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT) newPos += right * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT) newPos -= right * speed;

	if (App->input->GetMouseZ() > 0) newPos += front * speed * 2;
	if (App->input->GetMouseZ() < 0) newPos -= front * speed * 2;

	// Mouse motion ----------------

	bool hasRotated = false;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		{
			if (App->gameObjects->selectedGameObject != nullptr)
			{
				const float newDeltaX = (float)dx * cameraSensitivity;
				const float newDeltaY = (float)dy * cameraSensitivity;

				reference = App->gameObjects->selectedGameObject->GetComponent<Transform>()->GetPos();
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
	if (projectionIsDirty)
		RecalculateProjection();

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
	cameraFrustum.SetVerticalFovAndAspectRatio(verticalFOV * DEGTORAD, aspectRatio);
}

void ModuleCamera3D::OnSave(JSONWriter& writer) const
{
	writer.String("camera");
	writer.StartObject();
	SAVE_JSON_FLOAT(verticalFOV);
	SAVE_JSON_FLOAT(nearPlaneDistance);
	SAVE_JSON_FLOAT(farPlaneDistance);
	SAVE_JSON_FLOAT(cameraSpeed);
	SAVE_JSON_FLOAT(cameraSensitivity);
	writer.EndObject();
}

void ModuleCamera3D::OnLoad(const JSONReader& reader)
{
	if (reader.HasMember("camera"))
	{
		const auto& config = reader["camera"];
		LOAD_JSON_FLOAT(verticalFOV);
		LOAD_JSON_FLOAT(nearPlaneDistance);
		LOAD_JSON_FLOAT(farPlaneDistance);
		LOAD_JSON_FLOAT(cameraSpeed);
		LOAD_JSON_FLOAT(cameraSensitivity);
	}
	RecalculateProjection();
}