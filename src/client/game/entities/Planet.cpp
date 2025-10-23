// Planet.cpp
#include "Planet.h"
#include "../planetaryTrials/PlanetaryUtils.h"
#include <cmath>

Planet::Planet(GameWorld *world, const Vec2d &pos, double radius, SDL_Color color, const std::string &textureKey)
    : Entity(world, NORMAL_ENTITY, ERROR_ENTITY, pos, Vec2d(radius*2, radius*2), Vec2d(0,0), 1.0, false, 1.0),
      m_Radius(radius), m_Color(color), m_TextureKey(textureKey)
{
    if (!m_TextureKey.empty())
    {
        m_Texture = Assets::Get()->GetTexture(m_TextureKey);
    }
}

Planet::~Planet() = default;

void Planet::Tick()
{
    // Slowly rotate the planet over time
    const double rotation_speed = 0.18; // radians per second (~10 deg/s)
    // Use game tick to advance rotation if world has tick delta; fallback small increment
    m_Rotation += rotation_speed * (1.0 / 60.0);
    if (m_Rotation > 2.0 * M_PI) m_Rotation = fmod(m_Rotation, 2.0 * M_PI);
}

void Planet::Draw()
{
    Drawing *render = m_World->GameWindow()->Render();
    SDL_Renderer *renderer = m_World->GameWindow()->Renderer();

    // Prefer textured spherical render to match planetary world visuals
    if (m_Texture && m_Texture->SDLTexture())
    {
        // Build a simple ring of vertices (latitude bands) like PlanetaryWorld::RenderPlanetCore
        const int segments = 30;
        const int rings = 20;

        std::vector<SDL_Vertex> verts;
        std::vector<int> indices;
        std::vector<float> zdepth;

        double radius = m_Radius * render->GetZoom();

        // Convert world center to screen coordinates
        Vec2d screenCenter = PlanetaryUtils::WorldToScreen(m_Core.Pos,
                                                           Vec2d(render->GetCameraX(), render->GetCameraY()),
                                                           m_World->GameWindow()->GetWidth(),
                                                           m_World->GameWindow()->GetHeight(),
                                                           render->GetZoom());

        verts.reserve((rings + 1) * (segments + 2));
        zdepth.reserve((rings + 1) * (segments + 2));
        indices.reserve(rings * segments * 6);

        for (int ring = 0; ring <= rings; ++ring)
        {
            double phi = M_PI * ring / rings;
            phi = std::max(0.0, std::min(M_PI, phi));

            for (int seg = 0; seg <= segments; ++seg)
            {
                // Apply planet rotation as a longitude offset
                double theta = 2.0 * M_PI * seg / segments + m_Rotation;

                double x = std::sin(phi) * std::cos(theta);
                double y = std::cos(phi);
                double z = std::sin(phi) * std::sin(theta);

                double screen_x = screenCenter.x + x * radius;
                double screen_y = screenCenter.y - y * radius;

                // Simple lighting (brightened so planets stand out more)
                double light_dir_x = 0.3, light_dir_y = 0.3, light_dir_z = 0.8;
                double dot = x * light_dir_x + y * light_dir_y + z * light_dir_z;
                double light_intensity = std::max(0.25, dot * 0.95 + 0.25); // raised base and multiplier
                // Apply a subtle overall brightness boost
                light_intensity = std::min(1.0, light_intensity * 1.15);
                Uint8 brightness = static_cast<Uint8>(light_intensity * 255);

                float u = 1.0f - (float)seg / segments;
                float v = (float)ring / rings;

                SDL_Vertex vertex;
                vertex.position = {(float)screen_x, (float)screen_y};
                vertex.tex_coord = {u, v};
                vertex.color = {brightness, brightness, brightness, 255};

                verts.push_back(vertex);
                zdepth.push_back((float)z);
            }
        }

        int row_stride = segments + 1;
        const float kEps = 0.001f;

        auto tri_front = [&](int a, int b, int c) -> bool
        {
            float avgz = (zdepth[a] + zdepth[b] + zdepth[c]) / 3.0f;
            return avgz > -kEps;
        };

        for (int ring = 0; ring < rings; ++ring)
        {
            for (int seg = 0; seg < segments; ++seg)
            {
                int i0 = ring * row_stride + seg;
                int i1 = i0 + 1;
                int i2 = i0 + row_stride;
                int i3 = i2 + 1;

                if (tri_front(i0, i2, i1))
                {
                    indices.push_back(i0);
                    indices.push_back(i2);
                    indices.push_back(i1);
                }
                if (tri_front(i1, i2, i3))
                {
                    indices.push_back(i1);
                    indices.push_back(i2);
                    indices.push_back(i3);
                }
            }
        }

        // Render geometry using the planet texture
        SDL_RenderGeometry(renderer,
                           m_Texture->SDLTexture(),
                           verts.data(), (int)verts.size(),
                           indices.data(), (int)indices.size());

        return;
    }

    // Fallback: draw camera-aware filled circle
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    render->SetColor(m_Color);
    Vec2d screenCenter = PlanetaryUtils::WorldToScreen(m_Core.Pos,
                                                       Vec2d(render->GetCameraX(), render->GetCameraY()),
                                                       m_World->GameWindow()->GetWidth(),
                                                       m_World->GameWindow()->GetHeight(),
                                                       render->GetZoom());
    int cx = int(screenCenter.x);
    int cy = int(screenCenter.y);
    int radius = int(m_Radius * render->GetZoom());

    for (int y = -radius; y <= radius; ++y)
    {
        int dx = (int)std::sqrt(radius * radius - y * y);
        SDL_RenderDrawLine(renderer, cx - dx, cy + y, cx + dx, cy + y);
    }
}
