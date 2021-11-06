#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "camera";
	CalculateViewMatrix();

	x = vec3(1.0f, 0.0f, 0.0f);
	y = vec3(0.0f, 1.0f, 0.0f);
	z = vec3(0.0f, 0.0f, 1.0f);
	position = vec3(0.0f, 0.0f, 5.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);
	currentDist = 0.0f;
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Start()
{
	LOG_CONSOLE("Setting up the camera");
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	viewMatrix.~mat4x4();
	viewMatrixInverse.~mat4x4();

	return true;
}

UpdateStatus ModuleCamera3D::Update(float dt)
{
	vec3 newPos(0, 0, 0);
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_STATE::KEY_REPEAT)
	{
		float speed = 10.0f * dt;
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_STATE::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_STATE::KEY_REPEAT)
		{
			speed = 20.0f * dt;
		}

		// WASD keys -----------------------------------
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_STATE::KEY_REPEAT) newPos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_STATE::KEY_REPEAT) newPos.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_STATE::KEY_REPEAT) newPos -= z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_STATE::KEY_REPEAT) newPos += z * speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_STATE::KEY_REPEAT) newPos -= x * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_STATE::KEY_REPEAT) newPos += x * speed;

		// Mouse motion ----------------
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		position -= reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			x = rotate(x, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			y = rotate(y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			z = rotate(z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			y = rotate(y, DeltaY, x);
			z = rotate(z, DeltaY, x);

			if (y.y < 0.0f)
			{
				z = vec3(0.0f, z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				y = cross(z, x);
			}
		}
		position = reference + z * length(position);
		currentDist = length(position);
	}
	else
	{
		//Zoom with Mouse Scroll
		float zoomSpeed = 15.0f * dt;
		if (App->input->GetMouseZ() > 0)
		{
			currentDist -= zoomSpeed;
			position = reference + z * currentDist;
		}
		if (App->input->GetMouseZ() < 0)
		{
			currentDist += zoomSpeed;
			position = reference + z * currentDist;
		}
	}

	if (App->gameObjects->selectedGameObject != nullptr && App->input->GetKey(SDL_SCANCODE_F) == KEY_STATE::KEY_DOWN)
	{
		Transform* t = App->gameObjects->selectedGameObject->GetTransform();
		newPos = vec3(t->GetPos().x, t->GetPos().y, t->GetPos().z);
		LookAt(vec3(t->GetPos().x, t->GetPos().y, t->GetPos().z));
	}

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_STATE::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_STATE::KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_STATE::KEY_DOWN)
		{
			LOG_CONSOLE("Reset camera position")
				position = vec3(0.0f, 0.0f, 5.0f);
			App->camera->Move(vec3(1, 2, 1));
			App->camera->LookAt(vec3(0, 0, 0));
		}
	}

	if (App->gameObjects->selectedGameObject != nullptr)
	{
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_STATE::KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RALT) == KEY_STATE::KEY_REPEAT)
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_STATE::KEY_REPEAT)
			{
				// Mouse motion ----------------
				int dx = -App->input->GetMouseXMotion();
				int dy = -App->input->GetMouseYMotion();

				float Sensitivity = 0.25f;

				position -= reference;

				if (dx != 0)
				{
					float DeltaX = (float)dx * Sensitivity;

					x = rotate(x, DeltaX, vec3(0.0f, 1.0f, 0.0f));
					y = rotate(y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
					z = rotate(z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				}

				if (dy != 0)
				{
					float DeltaY = (float)dy * Sensitivity;

					y = rotate(y, DeltaY, x);
					z = rotate(z, DeltaY, x);

					if (y.y < 0.0f)
					{
						z = vec3(0.0f, z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
						y = cross(z, x);
					}
				}
				position = reference + z * length(position);
				currentDist = length(position);

				float3 focusedPos = App->gameObjects->selectedGameObject->GetTransform()->GetPos();
				LookAt(vec3(focusedPos.x, focusedPos.y, focusedPos.z));
			}
		}
	}

	position += newPos;
	reference += newPos;

	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleCamera3D::Look(const vec3& position, const vec3& reference, bool rotateAroundReference)
{
	this->position = position;
	this->reference = reference;

	z = normalize(position - reference);
	x = normalize(cross(vec3(0.0f, 1.0f, 0.0f), z));
	y = cross(z, x);

	if (!rotateAroundReference)
	{
		this->reference = this->position;
		this->position += z * 0.05f;
	}

	currentDist = length(position);

	CalculateViewMatrix();
}

void ModuleCamera3D::LookAt(const vec3& spot)
{
	reference = spot;

	z = normalize(position - reference);
	x = normalize(cross(vec3(0.0f, 1.0f, 0.0f), z));
	y = cross(z, x);

	currentDist = length(position);

	CalculateViewMatrix();
}

void ModuleCamera3D::Move(const vec3& movement)
{
	position += movement;
	reference += movement;

	currentDist = length(position);

	CalculateViewMatrix();
}

float* ModuleCamera3D::GetViewMatrix()
{
	return &viewMatrix;
}

void ModuleCamera3D::CalculateViewMatrix()
{
	viewMatrix = mat4x4(x.x, y.x, z.x, 0.0f, x.y, y.y, z.y, 0.0f, x.z, y.z, z.z, 0.0f, -dot(x, position), -dot(y, position), -dot(z, position), 1.0f);
	viewMatrixInverse = inverse(viewMatrix);
}