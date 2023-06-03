//
// Created by 11dpmbozko on 18.05.2023.
//

#ifndef TRIALANDERROR_CHARACTERNPC_H
#define TRIALANDERROR_CHARACTERNPC_H

#include "../Character.h"

enum NPCType {
    NPC_DUMMY,
    NPC_TURRET
};

class CharacterNPC : public Character {
protected:
    unsigned long long m_NPCLastShot;
    bool m_IsBoss;
    NPCType m_AIType;

    void TickControls() override;

public:
    CharacterNPC(GameWorld* world, double max_health,
                 double start_x, double start_y,
                 double start_xvel, double start_yvel,
                 NPCType ai_type, bool is_boss);
    ~CharacterNPC();

    // Ticking
    void Tick(); // TODO this is not good
};

#endif //TRIALANDERROR_CHARACTERNPC_H
