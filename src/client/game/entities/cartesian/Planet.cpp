// Copied Planet.cpp into cartesian
#include "Planet.h"
#include "../../../core/Assets.h"

Planet::Planet(GameWorld *world, const Vec2d& pos, double radius, SDL_Color color, const std::string& texture_path)
    : Entity(world, NORMAL_ENTITY, ITEM_ENTITY, pos, Vec2d(radius*2, radius*2), Vec2d(0,0), 1.0, false)
{
    m_Radius = radius;
    m_Color = color;
    m_TexturePath = texture_path;
    m_Texture = Assets::Get()->GetTexture(texture_path.c_str());
}

Planet::~Planet() = default;

void Planet::Draw()
{
    Drawing *Render = m_World->GameWindow()->Render();
    if (m_Texture)
    {
        SDL_Rect dst{int(m_Core.Pos.x - m_Radius), int(m_Core.Pos.y - m_Radius), int(m_Radius*2), int(m_Radius*2)};
        Render->RenderTexture(m_Texture->SDLTexture(), nullptr, dst);
    }
    else
    {
        Render->SetColor(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
        SDL_Rect r{int(m_Core.Pos.x - m_Radius), int(m_Core.Pos.y - m_Radius), int(m_Radius*2), int(m_Radius*2)};
        Render->FillRectCamera(r);
    }
}
