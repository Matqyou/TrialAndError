// Copied Crate.cpp into cartesian
#include <client/game/entities/cartesian/Crate.h>
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <iostream>

LoadedSound Crate::sHitSound[] = {
    LoadedSound("entity.crate.broken.1"),
    LoadedSound("entity.crate.broken.2"),
    LoadedSound("entity.crate.broken.3"),
};
LoadedSound Crate::sBoxSound("entity.crate.hurt");
LoadedTexture Crate::sBoxTexture("entity.crate");
LoadedTexture Crate::sBreakingBox1Texture("entity.crate2");
LoadedTexture Crate::sBreakingBox2Texture("entity.crate3");

Crate::Crate(GameWorld *world,
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
             20.0)
{
    m_World = world;
    m_Alive = true;
    m_DropType = RandomDrop;
    m_Texture = sBoxTexture.GetTexture();

    auto Random = m_World->GameWindow()->Random();
    float RandomFloat = Random->PercentageFloat();
    if (RandomFloat <= 1 / 8.0f) m_ErrorType = DISORIANTED;
    else if (RandomFloat <= 2 / 8.0f)
    { m_ErrorType = SPIKY; }
    else if (RandomFloat <= 3 / 8.0f)
    { m_ErrorType = CONFUSING_HP; }
    else if (RandomFloat <= 4 / 8.0f)
    { m_ErrorType = INVINCIBLE; }
    else if (RandomFloat <= 5 / 8.0f)
    { m_ErrorType = HEALERS_PARADISE; }
    else if (RandomFloat <= 6 / 8.0f)
    { m_ErrorType = RANGED; }
    else if (RandomFloat <= 7 / 8.0f)
    { m_ErrorType = SLOW_DOWN; }
    else if (RandomFloat <= 8 / 8.0f)
    { m_ErrorType = DANGEROUS_RECOIL; }
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

void Crate::Damage(double value, Entity *damager)
{
    Sound *BoxHitSound = sBoxSound.GetSound();
    BoxHitSound->PlaySound();
    m_HealthComponent.ChangeHealthBy(-value);

    if (m_HealthComponent.m_Health < 10) m_Texture = sBreakingBox2Texture.GetTexture();
    else if (m_HealthComponent.m_Health < 20) m_Texture = sBreakingBox1Texture.GetTexture();
}

void Crate::Heal(double value)
{
    m_HealthComponent.ChangeHealthBy(+value);
}

void Crate::Tick()
{
    TickWalls();

    if (!m_HealthComponent.IsAlive())
    {
        m_Alive = false;
        sHitSound[rand() % 3].GetSound()->PlaySound();
        if (m_DropType != ERROR)
        {
            auto Ammo_type = m_World->GameWindow()->Random()->UnsignedInt() % NUM_AMMO_TYPES;
            new AmmoBox(m_World, AmmoType(Ammo_type), m_Core.Pos, 20);
        }
        else
        {
            new Error(m_World,
                      m_Core.Pos,
                      m_ErrorType);
        }
    }
}

void Crate::Draw()
{
    Drawing *Render = m_World->GameWindow()->Render();

    SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
                           float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
                           float(m_Core.Size.x),
                           float(m_Core.Size.y) };

    Render->RenderTextureFCamera(m_Texture->SDLTexture(), nullptr, DrawRect);
}
