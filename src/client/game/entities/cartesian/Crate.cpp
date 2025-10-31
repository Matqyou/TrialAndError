// Copied Crate.cpp into cartesian
#include <client/game/entities/cartesian/Crate.h>
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <iostream>

LinkSound Crate::sHitSound[] = {
	LinkSound("entity.crate.broken.1"),
	LinkSound("entity.crate.broken.2"),
	LinkSound("entity.crate.broken.3"),
};
LinkSound Crate::sBoxSound("entity.crate.hurt");
LinkTexture Crate::sBoxTexture("entity.crate");
LinkTexture Crate::sBreakingBox1Texture("entity.crate2");
LinkTexture Crate::sBreakingBox2Texture("entity.crate3");

Crate::Crate(GameWorld *world,
             const Vec2f& start_pos,
             DropType RandomDrop)
    : Entity(world,
             NORMAL_ENTITY,
             CRATE_ENTITY,
             start_pos,
             Vec2f(50, 50),
             Vec2f(0.0, 0.0),
             0.95,
             true,
             20.0)
{
    m_World = world;
    m_Alive = true;
    m_DropType = RandomDrop;
    m_Texture = sBoxTexture.GetTexture();

    auto Random = Application.GetRandomizer();
    float RandomFloat = Random->PercentageFloat();
    if (RandomFloat <= 1 / 8.0f) m_ErrorType = DISORIENTED;
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
	auto characters = m_World->GetEntitiesByType(CHARACTER_ENTITY);
    for (auto entity : characters)
	{
		Character* Char = (Character*)entity;
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

void Crate::Tick(double elapsed_seconds)
{
    TickWalls();

    if (!m_HealthComponent.IsAlive())
    {
        m_Alive = false;
        sHitSound[rand() % 3].GetSound()->PlaySound();
        if (m_DropType != ERROR)
        {
            auto Ammo_type = Application.GetRandomizer()->UnsignedInt() % NUM_AMMO_TYPES;
            new AmmoBox(m_World, AmmoType(Ammo_type), m_Core.pos, 20);
        }
        else
        {
            new Error(m_World, m_Core.pos, m_ErrorType);
        }
    }
}

void Crate::Draw()
{
    auto Render = Application.GetDrawing();

    SDL_FRect DrawRect = { float(m_Core.pos.x) - float(m_Core.size.x / 2.0),
                           float(m_Core.pos.y) - float(m_Core.size.y / 2.0),
                           float(m_Core.size.x),
                           float(m_Core.size.y) };
	Render->RenderTexture(m_Texture->SDLTexture(), nullptr, DrawRect);
}
