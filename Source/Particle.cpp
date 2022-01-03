#include "Particle.h"
#include "ResourceMesh.h"
#include "Gameobject.h"

Particle::Particle(ResourceMesh* mesh, ResourceTexture* tex, float3 pos, Quat rot, float3 scale)
{
    //color.clear();
    //plane = mesh;

    //lifeTime = CreateRandomNum(data.life);

    //life = 0.0f;

    //speed = CreateRandomNum(data.speed);
    //acceleration = CreateRandomNum(data.acceleration);
    //direction = data.particleDirection;

    //angle = CreateRandomNum(data.rotation) * DEGTORAD;
    //angularVelocity = CreateRandomNum(data.angularVelocity) * DEGTORAD;
    //angularAcceleration = CreateRandomNum(data.angularAcceleration) * DEGTORAD;

    //sizeOverTime = CreateRandomNum(data.sizeOverTime);

    //transform.position = pos;
    //transform.rotation = Quat::FromEulerXYZ(0, 0, 0); //Start rotation
    //transform.scale = float3::one * CreateRandomNum(data.size);

    ////LOG("life %f", lifeTime);
    ////LOG("size %f", transform.scale.x);

    //for (std::list<ColorTime>::iterator iter = data.color.begin(); iter != data.color.end(); ++iter)
    //    color.push_back(*iter);

    //multicolor = data.timeColor;
    //this->texture = texture;


    //this->animation = animation;
    //this->animationSpeed = animationSpeed;
    //animationTime = 0.0f;
    //currentFrame = 0u;

    //active = true;
    //subEmitterActive = data.subEmitterActive;
    //index = 0;

    //App->particle->activeParticles++;
}

Particle::Particle()
{

}

Particle::~Particle()
{
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
    }
    else
    {
        EndParticle(ret);
    }
    return ret;
}

bool Particle::Draw()
{
    return false;
}

float4x4 Particle::GetMatrix() const
{
    return float4x4::FromTRS(pos, rot, scale).Transposed();
}

void Particle::EndParticle(bool& ret)
{
    active = false;
    ret = false;
    for (std::vector<Particle*>::iterator it = owner->particlePool.begin(); it != owner->particlePool.end(); ++it)
    {
        if (this == (*it))
        {
            owner->particlePool.erase(it);
            owner = nullptr;
        }
    }
}
