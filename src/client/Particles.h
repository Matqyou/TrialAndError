//
// Created by Matq on 10/12/2024.
//

#pragma once

#include "../technical stuff/Drawing.h"
#include "../technical stuff/Vec2.h"

struct Particle {
    Texture* m_Texture;
    Vec2d m_Position;
    Vec2d m_Size;
    Vec2d m_Vel;
    double m_VelDamping;
    double m_Orientation;
    double m_OrientationVel;
    double m_OrientationVelDamping;
    unsigned long long m_Lifetime;
    bool m_Active;

    Particle() {
        m_Texture = nullptr;
        m_Position = {};
        m_Size = {};
        m_Vel = {};
        m_Orientation = {};
        m_OrientationVel = {};
        m_Active = false;
    }
    Particle(Texture* texture, const Vec2d& pos, const Vec2d& size, const Vec2d& vel, double vel_damping, double orientation, double orientation_vel, double orientation_vel_damping, unsigned long long lifetime) {
        m_Texture = texture;
        m_Position = pos;
        m_Size = size;
        m_Vel = vel;
        m_VelDamping = vel_damping;
        m_Orientation = orientation;
        m_OrientationVel = orientation_vel;
        m_OrientationVelDamping = orientation_vel_damping;
        m_Lifetime = lifetime;

        m_Active = true;
    }

    void Tick();
    void Draw(Drawing* drawing) const;

};

class GameWorld;
class Particles {
private:
    GameWorld* m_World;
    Drawing* m_Drawing;
    Particle m_Particles[1024];
    size_t m_CreateParticleIndex;

public:
    explicit Particles(GameWorld* game_world);

    // Manipulating
    bool PlayParticle(Particle particle);

    // Ticking
    void Tick();
    void Draw();

};

