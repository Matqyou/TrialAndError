//
// Created by Mim on 5/8/2023.
//
#include "characters/character/Character.h"
#include "Crate.h"
#include <iostream>
Texture* Crate::ms_TextureBox = nullptr;
Texture* Crate::ms_TextureBreakingBox1 = nullptr;
Texture* Crate::ms_TextureBreakingBox2 = nullptr;
Sound* Crate::ms_HitSound = nullptr;
Sound* Crate::ms_BoxSound = nullptr;

Crate::Crate(GameWorld* world,
             const Vec2d& start_pos,
             DropType RandomDrop)
 : Entity(world,
             NORMAL_ENTITY,
             CRATE_ENTITY,
             start_pos,
             Vec2d(50, 50),
             Vec2d(0.0, 0.0),
             0.95,
             true,
             20.0) {
    m_World = world;
    m_Alive = true;
    m_DropType = RandomDrop;
    m_Texture = &ms_TextureBox;

    auto Random = m_World->GameWindow()->Random();
    float RandomFloat = Random->PercentageFloat();
    if (RandomFloat <= 1 / 8.0f) m_ErrorType = DISORIANTED;
    else if (RandomFloat <= 2 / 8.0f) { m_ErrorType = SPIKY; }
    else if (RandomFloat <= 3 / 8.0f) { m_ErrorType = CONFUSING_HP; }
    else if (RandomFloat <= 4 / 8.0f) { m_ErrorType = INVINCIBLE; }
    else if (RandomFloat <= 5 / 8.0f) { m_ErrorType = HEALERS_PARADISE; }
    else if (RandomFloat <= 6 / 8.0f) { m_ErrorType = RANGED; }
    else if (RandomFloat <= 7 / 8.0f) { m_ErrorType = SLOW_DOWN; }
    else if (RandomFloat <= 8 / 8.0f) { m_ErrorType = DANGEROUS_RECOIL; }
}

Crate::~Crate()
{
    Character *Char = m_World->FirstCharacter();
    for (; Char; Char = (Character *)Char->NextType())
    {
        Hook *TargetHook = Char->GetHook();
        if (TargetHook->m_GrabbedEntity == this)
            TargetHook->Unhook();
    }
}

void Crate::Damage(double value, Entity* damager) {
    Sound* BoxHitSound = ms_BoxSound;
    m_World->GameWindow()->Assetz()->SoundHandler()->PlaySound(BoxHitSound);
    m_HealthComponent.ChangeHealthBy(-value);

    if (m_HealthComponent.m_Health < 10) m_Texture = &ms_TextureBreakingBox2;
    else if (m_HealthComponent.m_Health < 20) m_Texture = &ms_TextureBreakingBox1;
}

void Crate::Heal(double value) {
    m_HealthComponent.ChangeHealthBy(+value);
};

void Crate::Tick() {
    TickWalls();

    // Die
    if (!m_HealthComponent.IsAlive()) {
        m_Alive = false;
        m_World->GameWindow()->Assetz()->SoundHandler()->PlaySound(ms_HitSound);
        if (m_DropType != ERROR) {
            auto Ammo_type = m_World->GameWindow()->Random()->UnsignedInt() % 4;
            new AmmoBox(m_World, AmmoType(Ammo_type), m_Core.Pos, 20);
        } else {
            new Error(m_World,
                      m_Core.Pos,
                      m_ErrorType); // To change the drop just change m_ErrorType the enum value of whatever ERROR is needed
        }
    }
}

void Crate::Draw() {
    
    Drawing* Render = m_World->GameWindow()->Render();

    SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
                           float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
                           float(m_Core.Size.x),
                           float(m_Core.Size.y) };

    Render->RenderTextureFCamera((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}
