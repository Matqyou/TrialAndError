//
// Created by 11dpmbozko on 18.05.2023.
//

#ifndef TRIALANDERROR_CHARACTERNPC_H
#define TRIALANDERROR_CHARACTERNPC_H

#include "../Character.h"

class CharacterNPC : Character {
private:
    unsigned long long m_NPCLastShot;

public:
    CharacterNPC(GameWorld* world, double max_health,
                 double start_x, double start_y,
                 double start_xvel, double start_yvel);
    ~CharacterNPC();

    void TickControls() override;
};

#endif //TRIALANDERROR_CHARACTERNPC_H
