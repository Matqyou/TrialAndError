//
// Created by Matq on 14/04/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_CHARACTER_HOOK_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_CHARACTER_HOOK_H_

#include "../Entity.h"

class Character;
struct Hook {
    enum {
        GRABBED_NONE,
        GRABBED_ENTITY,
        GRABBED_WALL
    };

    Character* m_Parent;
    double m_x, m_y;
    double m_xvel, m_yvel;
    double m_MaxLength;
    double m_HookTravelSpeed;
    double m_WallDragForce;
    double m_HookerInfluenceRatio;
    double m_HookistInfluenceRatio;
    bool m_Deployed;
    int m_Grabbed;
    Entity* m_GrabbedEntity;

    Hook(Character* parent);

    void SetInfluenceRatio(double ratio);
    void Unhook();
    void HookWall();

    void Tick(bool hooking, bool last_hooking);
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_CHARACTER_HOOK_H_
