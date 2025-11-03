// Copied AmmoBox.cpp into cartesian
#include <client/game/entities/cartesian/AmmoBox.h>
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <cmath>
#include <iostream>

LinkTexture AmmoBox::sTextureGlock("entity.ammo_box.glock");
LinkTexture AmmoBox::sTextureShotgun("entity.ammo_box.shotgun");
LinkTexture AmmoBox::sTextureBurst("entity.ammo_box.burst");
LinkTexture AmmoBox::sTextureMinigun("entity.ammo_box.minigun");
LinkTexture AmmoBox::sTextureSniper("entity.ammo_box.sniper");

AmmoBox::AmmoBox(AmmoType type,
                 const Vec2f& start_pos,
                 unsigned int AmmoCount)
    : Entity(NORMAL_ENTITY,
			 ENTITY_AMMO_BOX,
			 start_pos,
			 Vec2f(40, 28),
			 Vec2f(0.0, 0.0),
			 0.95,
			 false)
{
    m_AmmoCount = AmmoCount;
    m_Type = type;

    if (type == AMMO_GLOCK) m_Texture = sTextureGlock.GetTexture();
    else if (type == AMMO_SHOTGUN) m_Texture = sTextureShotgun.GetTexture();
    else if (type == AMMO_BURST) m_Texture = sTextureBurst.GetTexture();
    else if (type == AMMO_MINIGUN) m_Texture = sTextureMinigun.GetTexture();
    else if (type == AMMO_SNIPER) m_Texture = sTextureSniper.GetTexture();
}
void AmmoBox::TickPickup()
{
    auto characters = world->GetEntitiesByType(ENTITY_CHARACTER);
    for (Entity* entity : characters)
    {
		Character* character = (Character*)entity;
        EntityCore& CharCore = character->GetCore();

        float Distance = (core.pos - CharCore.pos).LengthF();
        if (Distance > core.size_ratio + character->GetCore().size_ratio)
			continue;

        character->AmmoPickup(this);
    }
}

unsigned int AmmoBox::TakeAmmo(unsigned int amount)
{
    if (amount > m_AmmoCount)
		amount = m_AmmoCount;

    m_AmmoCount -= amount;
    return amount;
}

void AmmoBox::Tick(double elapsed_seconds)
{
    TickPickup();

    TickVelocity(elapsed_seconds);
    TickWalls();

    if (m_AmmoCount <= 0) alive = false;
}

void AmmoBox::Draw()
{
    Drawing *drawing = Application.GetDrawing();

    SDL_FRect DrawRect = { float(core.pos.x) - float(core.size.x / 2.0),
                           float(core.pos.y) - float(core.size.y / 2.0),
                           float(core.size.x),
                           float(core.size.y) };
    drawing->RenderTexture(m_Texture->SDLTexture(), nullptr, DrawRect, GameReference.GetCamera());
}
