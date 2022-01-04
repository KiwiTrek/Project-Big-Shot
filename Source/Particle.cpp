#include "Particle.h"
#include "RenderGlobals.h"
#include "Gameobject.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

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

    speed = vel;
    acceleration = acc;
    this->direction = direction;

    this->angle = angle * DEG_TO_RAD;
    angularVelocity = angularVel * DEG_TO_RAD;
    angularAcceleration = angularAcc * DEG_TO_RAD;

    sizeOverTime = incrementSize;

    //LOG("life %f", lifeTime);
    //LOG("size %f", transform.scale.x);

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

bool Particle::Update(float dt)
{
    bool ret = true;
    life += dt;
    if (life < maxLife)
    {
        speed += acceleration * dt;
        pos += direction * speed * dt;

        //look at camera

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
                if (color[index + 1].pos == 0)
                {
                    timeNormalized = 0;
                }
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
        rot = rot.RotateY(180 * DEG_TO_RAD);
    }
    else
    {
        active = false;
    }
    return ret;
}

void Particle::Draw()
{
    if (plane == nullptr) return;

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    //vertices
    glBindBuffer(GL_ARRAY_BUFFER, plane->vertexBuf);
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    //normals
    glBindBuffer(GL_NORMAL_ARRAY, plane->normalsBuf);
    glNormalPointer(GL_FLOAT, 0, NULL);

    //indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane->indexBuf);

    if (texture != nullptr)
    {
        //textures
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindBuffer(GL_ARRAY_BUFFER, plane->textureBuf);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);

        glBindTexture(GL_TEXTURE_2D, texture->texId);
    }

    float4x4 t = float4x4::FromTRS(pos, rot, scale);

    glPushMatrix();
    glMultMatrixf((float*)&t.Transposed());

    glColor3f(currentColor.x, currentColor.y, currentColor.z);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawElements(GL_TRIANGLES, plane->indexNum, GL_UNSIGNED_INT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    glPopMatrix();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_NORMAL_ARRAY, 0);
    glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

float4x4 Particle::GetMatrix()
{
    return float4x4::FromTRS(pos, rot, scale).Transposed();
}