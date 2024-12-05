//
// Created by 11dpmbozko on 18.05.2023.
//

#pragma once

#include "character/Character.h"
class Character;
enum NPCType {
    NPC_DUMMY,
    NPC_TURRET
};

class CharacterNPC : public Character {
protected:
    
    bool m_IsBoss;
    Player* m_LastAttacker;
    NPCType m_AIType;

    // Listening & Ticking
    void EventDeath() override;
    void TickControls() override;
    
    

public:
    CharacterNPC(GameWorld* world,
                 double max_health,
                 const Vec2d& start_pos,
                 const Vec2d& start_vel,
                 NPCType ai_type,
                 bool is_boss);
    ~CharacterNPC();
    void UpdateAttacker(Player* attacker);
    [[nodiscard]] bool IsBoss() const { return m_IsBoss; }
};
