//
// Created by janis on 3/22/2023.
//

#ifndef TRIALANDERROR_BULLETS_H
#define TRIALANDERROR_BULLETS_H

#include "Entity.h"
#include "SDL.h"

class Bullets : public Entity {
private:
    Entity* m_Shooter;
    double m_Damage;
    bool m_StillCollidesShooter;

    void TickImpact();
    bool TickVelocity();
    bool TickHitPoint(double x, double y);

public:
    static Texture* ms_Texture;

    Bullets(GameWorld* world, Entity* shooter, double damage, double start_x, double start_y, double start_xvel, double start_yvel);
    void Tick() override;
    void Draw() override;
};

#endif //TRIALANDERROR_BULLETS_H
