#pragma once

#include "../../planetary/PlanetaryUtils.h"
#include "../../../../shared/utility/Vec2.h"
#include "../../GameWorld.h"

// Minimal planetary entity core and base classes (shim)
struct PlanetaryEntityCore
{
    PlanetaryCoords Pos; // spherical coords
    Vec2d Size; // display size
    Vec2d Vel; // in Cartesian space or angular deltas if desired
    double BaseDamping{0};
    double sizeRatio{0};

    Vec2d ToCartesian() const { return Pos.ToCartesian(); }
};

struct PlanetaryDirectionalCore : public PlanetaryEntityCore
{
    Vec2d Direction; // direction in Cartesian tangent space
    PlanetaryDirectionalCore() : PlanetaryEntityCore() { }
};

class PlanetaryEntity
{
protected:
    GameWorld* m_World;
    PlanetaryEntity* m_Prev; 
    PlanetaryEntity* m_Next;

    PlanetaryEntityCore m_Core;
    PlanetaryEntityCore m_LastCore;
    bool m_Alive;

public:
    PlanetaryEntity(GameWorld* world,
                    const PlanetaryCoords& start_pos,
                    const Vec2d& start_size,
                    const Vec2d& start_vel);
    virtual ~PlanetaryEntity();

    // Getting
    PlanetaryEntity* Next() const { return m_Next; }
    PlanetaryEntity* Prev() const { return m_Prev; }
    PlanetaryEntityCore& GetCore() { return m_Core; }
    PlanetaryEntityCore& GetLastCore() { return m_LastCore; }
    GameWorld* World() const { return m_World; }

    // Ticking
    virtual void Tick();
    virtual void Draw();
};

class PlanetaryDirectionalEntity : public PlanetaryEntity
{
protected:
    PlanetaryDirectionalCore m_DirectionalCore;
    PlanetaryDirectionalCore m_LastDirectionalCore;

public:
    PlanetaryDirectionalEntity(GameWorld* world,
                               const PlanetaryCoords& start_pos,
                               const Vec2d& start_size,
                               const Vec2d& start_vel,
                               const Vec2d& start_direction);
    ~PlanetaryDirectionalEntity() override;

    PlanetaryDirectionalCore& GetDirectionalCore() { return m_DirectionalCore; }
    PlanetaryDirectionalCore& GetLastDirectionalCore() { return m_LastDirectionalCore; }
};
