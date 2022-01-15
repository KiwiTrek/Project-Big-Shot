#include "Particle.h"
#include "RenderGlobals.h"
#include "Gameobject.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "Application.h"
#include "ModuleGameObjects.h"

Particle::Particle(ResourceMesh* mesh, ResourceMaterial* tex, float lifeTime, float3 pos, float3 scale, float angle, float acc,float vel,float3 direction, float incrementSize, float angularAcc, float angularVel, std::vector<FadeColor> colors)
{
    color.clear();
    plane = mesh;
    texture = tex;

    maxLife = lifeTime;
    life = 0.0f;

    this->pos = pos;
    rot = Quat::FromEulerXYZ(0, 0, 0);
    this->scale = scale;
    bbox.SetFromCenterAndSize(pos, scale);

    speed = vel;
    acceleration = acc;
    this->direction = direction;

    this->angle = angle * DEG_TO_RAD;
    angularVelocity = angularVel * DEG_TO_RAD;
    angularAcceleration = angularAcc * DEG_TO_RAD;

    sizeOverTime = incrementSize;

    for (std::vector<FadeColor>::iterator it = colors.begin(); it != colors.end(); ++it)
    {
        color.push_back(*it);
    }

    colors.size() > 1 ? multiColor = true : multiColor = false;
    index = 0;

    camDistance = 0.0f;
    active = true;
}

Particle::Particle()
{
    life = 0.0f;
    maxLife = 0.0f;

    pos = float3(0.0f, 0.0f, 0.0f);
    rot = Quat::FromEulerXYZ(0, 0, 0);
    angle = 0.0f;
    scale = float3(0.0f, 0.0f, 0.0f);
    bbox.SetFromCenterAndSize(pos, scale);

    acceleration = 0.0f;
    speed = 0.0f;

    angularAcceleration = 0.0f;
    angularVelocity = 0.0f;

    direction = float3(0.0f, 0.0f, 0.0f);
    angle = 0.0f;

    sizeOverTime = 0.0f;

    color.clear();
    index = 0;
    multiColor = false;
    currentColor = Color();

    plane = nullptr;
    texture = nullptr;
    camDistance = 0.0f;
    active = true;
}

Particle::~Particle()
{
    color.clear();
    plane = nullptr;
    texture = nullptr;
}

bool Particle::Update(float dt, Application* App)
{
    bool ret = true;
    life += dt;
    if (life < maxLife)
    {
        speed += acceleration * dt;
        pos += direction * speed * dt;

        float3 z = -App->gameObjects->mainCamera->GetComponent<Camera>()->frustum.Front();
        float3 y = -App->gameObjects->mainCamera->GetComponent<Camera>()->frustum.Up();
        float3 x = y.Cross(z).Normalized();

        rot.Set(float3x3(x, y, z));

        if (color.size() == 1 || !multiColor)
        {
            currentColor = color.front().color;
        }
        else if (index + 1 < color.size())
        {
            float lifeNormalized = life / maxLife;
            if (color[index + 1].pos > lifeNormalized)
            {
                float timeNormalized = (lifeNormalized - color[index].pos / (color[index + 1].pos - color[index].pos));
                if (color[index + 1].pos == 0) timeNormalized = 0;
                currentColor = color[index].color.Lerp(color[index + 1].color, timeNormalized);
            }
            else
            {
                index++;
            }
        }
        else
        {
            currentColor = color[index].color;
        }

        scale.x += sizeOverTime * dt;
        scale.y += sizeOverTime * dt;
        scale.z += sizeOverTime * dt;
        angularVelocity += angularAcceleration * dt;
        angle += angularVelocity * dt;
        rot = rot.Mul(Quat::RotateZ(angle));
        bbox.SetFromCenterAndSize(pos, scale);
    }
    else
    {
        ParticleDeath();
    }

    camDistance = App->gameObjects->mainCamera->GetComponent<Transform>()->GetPos().DistanceSq(pos);

    return ret;
}

void Particle::Draw()
{
    if (plane == nullptr) return;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    //Vertices
    glBindBuffer(GL_ARRAY_BUFFER, plane->vertexBuf);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    //Normals
    glBindBuffer(GL_NORMAL_ARRAY, plane->normalsBuf);
    glNormalPointer(GL_FLOAT, 0, NULL);

    //Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane->indexBuf);

    glColor4f(currentColor.x, currentColor.y, currentColor.z, currentColor.w);

    if (texture != nullptr)
    {
        //Textures
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glBindBuffer(GL_ARRAY_BUFFER, plane->textureBuf);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);

        glBindTexture(GL_TEXTURE_2D, texture->texId);
    }
    else
    {
        glDisable(GL_BLEND);
        glDisable(GL_ALPHA_TEST);
    }

    float4x4 t = float4x4::FromTRS(pos, rot, scale);

    glPushMatrix();
    glMultMatrixf((float*)&t.Transposed());

    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawElements(GL_TRIANGLES, plane->indexNum, GL_UNSIGNED_INT, NULL);

    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    glBindTexture(GL_TEXTURE_2D, 0);

    glPopMatrix();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_NORMAL_ARRAY, 0);
    glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Particle::ParticleDeath()
{
    if (active && owner->data.subEmitterActive && texture == owner->data.texture)
    {
        owner->newPositions.push_back(pos - owner->owner->GetComponent<Transform>()->GetPos());
    }
    active = false;
    camDistance = -1;
}