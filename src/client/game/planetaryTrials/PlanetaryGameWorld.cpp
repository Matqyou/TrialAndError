// PlanetarySystem.cpp
#include "client/GameData.h"
#include <random>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "PlanetaryUtils.h"
#include "PlanetaryGameWorld.h"
#include "client/game/entities/characters/CharacterNPC.h"
#include "PlanetaryCharacter.h"

// Simple 3D vector struct for local use
struct Vec3
{
    double x, y, z;
    Vec3(double x_ = 0, double y_ = 0, double z_ = 0) : x(x_), y(y_), z(z_) {}
};

// PlanetaryWorld Implementation
PlanetaryWorld::PlanetaryWorld(const PlanetConfig &config)
    : m_Config(config), m_CameraPos(0, 0), m_CameraZoom(1.0),
      m_RenderSegments(64), m_LastCameraAngle(0)
{
    m_Config = config;
    if (config.texture_path)
    {
        LoadPlanetTexture(config.texture_path);
    }
}

PlanetaryWorld::~PlanetaryWorld()
{
    // Cleanup handled by LoadedTexture destructors
}

void PlanetaryWorld::SetConfig(const PlanetConfig &config)
{
    m_Config = config;
    if (config.texture_path)
    {
        LoadPlanetTexture(config.texture_path);
    }
}

void PlanetaryWorld::AddAngles(double longChange, double latChange)
{
    m_Yaw = std::fmod(m_Yaw + longChange, 2.0 * M_PI);
    m_Pitch = std::fmod(m_Pitch + latChange, 2.0 * M_PI);
}

bool PlanetaryWorld::LoadPlanetTexture(const char *texture_path)
{
    m_PlanetTexture = Assets::Get()->GetTexture(texture_path);
    return m_PlanetTexture != nullptr;
}

bool PlanetaryWorld::LoadAtmosphereTexture(const char *texture_path)
{
    m_AtmosphereTexture = Assets::Get()->GetTexture(texture_path);
    return m_AtmosphereTexture != nullptr;
}

void PlanetaryWorld::AddBackgroundLayer(const char *texture_path, double distance)
{
    Texture *layer = Assets::Get()->GetTexture(texture_path);
    if (layer)
    {
        m_BackgroundLayers.push_back(layer);
        m_LayerDistances.push_back(distance);
    }
}

double PlanetaryWorld::GetSurfaceHeight(double angle) const
{
    // Basic implementation - can be enhanced with noise for terrain
    return m_Config.radius;
}

Vec2d PlanetaryWorld::GetSurfaceNormal(const Vec2d &world_pos) const
{
    // Surface normal points away from planet center
    Vec2d normal = world_pos;
    normal.Normalize();
    return normal;
}

void PlanetaryWorld::RenderPlanet(SDL_Renderer *renderer, int screen_width, int screen_height)
{
    RenderPlanetCore(renderer, screen_width, screen_height);
}

void PlanetaryWorld::RenderPlanetCore(SDL_Renderer *renderer, int screen_width, int screen_height)
{
    if (!m_PlanetTexture)
        return;

    const int segments = 30;
    const int rings = 40;

    std::vector<SDL_Vertex> verts;
    std::vector<int> indices;
    std::vector<float> zdepth;

    double radius = m_Config.radius;
    float center_x = screen_width / 2.0f;
    float center_y = (screen_height + radius) / 2.0f;

    double longitude_rotation = m_Yaw;
    double latitude_rotation = m_Pitch;

    verts.reserve((rings + 1) * (segments + 2));
    zdepth.reserve((rings + 1) * (segments + 2));
    indices.reserve(rings * segments * 6);

    for (int ring = 0; ring <= rings; ++ring)
    {
        double phi = M_PI * ring / rings;
        phi = std::max(0.0, std::min(M_PI, phi));

        for (int seg = 0; seg <= segments; ++seg)
        {
            double theta = 2.0 * M_PI * seg / segments;

            double x = sin(phi) * cos(theta);
            double y = cos(phi);
            double z = sin(phi) * sin(theta);

            // Rotate around Y (longitude)
            double x1 = x * cos(longitude_rotation) - z * sin(longitude_rotation);
            double y1 = y;
            double z1 = x * sin(longitude_rotation) + z * cos(longitude_rotation);

            // Rotate around X (latitude)
            double x2 = x1;
            double y2 = y1 * cos(latitude_rotation) - z1 * sin(latitude_rotation);
            double z2 = y1 * sin(latitude_rotation) + z1 * cos(latitude_rotation);

            double screen_x = center_x + x2 * radius;
            double screen_y = center_y - y2 * radius;

            // Lighting
            double light_dir_x = 0.3, light_dir_y = 0.3, light_dir_z = 0.8;
            double dot = x2 * light_dir_x + y2 * light_dir_y + z2 * light_dir_z;
            double light_intensity = std::max(0.15, dot * 0.85 + 0.15);
            Uint8 brightness = static_cast<Uint8>(light_intensity * 255);

            float u = 1.0f - (float)seg / segments;

            float v = (float)ring / rings;

            SDL_Vertex vertex;
            vertex.position = {(float)screen_x, (float)screen_y};
            vertex.tex_coord = {u, v};
            vertex.color = {brightness, brightness, brightness, 255};

            verts.push_back(vertex);
            zdepth.push_back((float)z2);
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
    SDL_RenderGeometry(renderer,
                       m_PlanetTexture->SDLTexture(),
                       verts.data(), (int)verts.size(),
                       indices.data(), (int)indices.size());
}

void PlanetaryWorld::RenderAtmosphere(SDL_Renderer *renderer, int screen_width, int screen_height)
{
    if (m_Config.atmosphere_height <= 0)
        return;

    // Render atmospheric glow
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    Vec2d planet_screen_pos = PlanetaryUtils::WorldToScreen(
        Vec2d(0, 0), m_CameraPos, screen_width, screen_height, m_CameraZoom);

    double atmosphere_radius = (m_Config.radius + m_Config.atmosphere_height) * m_CameraZoom;

    // Simple atmospheric glow effect
    for (int layer = 0; layer < 5; layer++)
    {
        double layer_radius = atmosphere_radius * (1.0 - layer * 0.1);
        int alpha = m_Config.atmosphere_color.a * (1.0 - layer * 0.2);

        SDL_SetRenderDrawColor(renderer,
                               m_Config.atmosphere_color.r,
                               m_Config.atmosphere_color.g,
                               m_Config.atmosphere_color.b,
                               alpha);

        // Draw atmospheric ring (simplified)
        for (int angle = 0; angle < 360; angle += 5)
        {
            double rad = angle * M_PI / 180.0;
            int x = (int)(planet_screen_pos.x + cos(rad) * layer_radius);
            int y = (int)(planet_screen_pos.y + sin(rad) * layer_radius);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void PlanetaryWorld::RenderBackgroundLayers(SDL_Renderer *renderer, int screen_width, int screen_height)
{
    // Render distant background layers with parallax
    for (size_t i = 0; i < m_BackgroundLayers.size(); i++)
    {
        double parallax_factor = 1.0 / m_LayerDistances[i];
        Vec2d layer_offset = m_CameraPos * parallax_factor;

        // Tile the background layer
        int tile_width = m_BackgroundLayers[i]->GetWidth();
        int tile_height = m_BackgroundLayers[i]->GetHeight();

        int start_x = (int)(layer_offset.x / tile_width) - 1;
        int start_y = (int)(layer_offset.y / tile_height) - 1;
        int end_x = start_x + (screen_width / tile_width) + 3;
        int end_y = start_y + (screen_height / tile_height) + 3;

        for (int ty = start_y; ty < end_y; ty++)
        {
            for (int tx = start_x; tx < end_x; tx++)
            {
                SDL_Rect dst_rect = {
                    tx * tile_width - (int)layer_offset.x,
                    ty * tile_height - (int)layer_offset.y,
                    tile_width, tile_height};
                SDL_RenderCopy(renderer, m_BackgroundLayers[i]->SDLTexture(), nullptr, &dst_rect);
            }
        }
    }
}

void PlanetaryWorld::UpdateCamera(const Vec2d &target_pos, double zoom_level)
{
    m_CameraPos = target_pos;
    m_CameraZoom = zoom_level;
}

PlanetaryCoords PlanetaryWorld::WorldToPlanetary(const Vec2d &world_pos) const
{
    return PlanetaryCoords::FromCartesian(world_pos, m_Config.radius);
}

Vec2d PlanetaryWorld::PlanetaryToWorld(const PlanetaryCoords &coords) const
{
    return coords.ToCartesian();
}

double PlanetaryWorld::GetDistanceFromSurface(const Vec2d &world_pos) const
{
    return world_pos.Length() - m_Config.radius;
}

PlanetaryGameWorld::PlanetaryGameWorld(GameData *game_window, int width, int height)
    : GameWorld(game_window, width, height), m_CurrentPlanet(nullptr), m_CurrentPlanetIndex(0)
{
    // Initialize with a default planet configuration
    PlanetConfig defaultPlanet;
    defaultPlanet.radius = width * height / 2;
    defaultPlanet.atmosphere_height = 100.0;
    defaultPlanet.surface_color = {100, 150, 200, 255};
    defaultPlanet.atmosphere_color = {150, 200, 255, 80};
    defaultPlanet.texture_path = "backgrounds.planet2";

    AddPlanetConfig(defaultPlanet);
    SetCurrentPlanet(0);
}

PlanetaryGameWorld::~PlanetaryGameWorld()
{
    delete m_CurrentPlanet;
}

void PlanetaryGameWorld::SetCurrentPlanet(int planet_index)
{
    if (planet_index < 0 || planet_index >= (int)m_AvailablePlanets.size())
        return;

    m_CurrentPlanetIndex = planet_index;

    // Clean up old planet
    delete m_CurrentPlanet;

    // Create new planet
    m_CurrentPlanet = new PlanetaryWorld(m_AvailablePlanets[planet_index]);

    // Add some background layers for variety
    if (planet_index == 0)
    {
        // Earth-like planet
        m_CurrentPlanet->AddBackgroundLayer("backgrounds.background_pattern", 10.0);
        m_CurrentPlanet->AddBackgroundLayer("backgrounds.background_pattern", 5.0);
    }
    else if (planet_index == 1)
    {
        // Mars-like planet
        m_CurrentPlanet->AddBackgroundLayer("assets/backgrounds/red_stars.png", 8.0);
    }
    // Add more planet-specific backgrounds as needed
}

void PlanetaryGameWorld::AddPlanetConfig(const PlanetConfig &config)
{
    m_AvailablePlanets.push_back(config);
}

void PlanetaryGameWorld::NextPlanet()
{
    if (m_CurrentPlanetIndex < (int)m_AvailablePlanets.size() - 1)
    {
        SetCurrentPlanet(m_CurrentPlanetIndex + 1);
    }
}

void PlanetaryGameWorld::PreviousPlanet()
{
    if (m_CurrentPlanetIndex > 0)
    {
        SetCurrentPlanet(m_CurrentPlanetIndex - 1);
    }
}

void PlanetaryGameWorld::TickPlanetaryCamera()
{
    if (!m_CurrentPlanet)
        return;

    PlanetaryCharacter* player = dynamic_cast<PlanetaryCharacter*>(FirstCharacter());
    if (!player || player->IsNPC())
        return;

    // Player world position from spherical coords
    const PlanetaryCoords& playerCoords = player->GetPlanetaryPos();
    Vec2d playerWorld = playerCoords.ToCartesian();

    Vec2d lookDir = player->GetDirectionalCore().Direction;

    const double radius = m_CurrentPlanet->GetConfig().radius;
    Vec2d targetCam = { playerWorld.x + lookDir.x ,
                        (playerWorld.y + lookDir.y) * 0.75};

    Drawing* render = m_GameWindow->Render();
    double oldCamX = render->GetCameraX();
    double oldCamY = render->GetCameraY();

    // Smooth follow (same 0.1 factor as your regular camera)
    const double followSpeed = 0.05;
    double newCamX = oldCamX + (targetCam.x - oldCamX) * followSpeed;
    double newCamY = oldCamY + (targetCam.y - oldCamY) * followSpeed;
    render->SetCameraPos(newCamX, newCamY);
}

void PlanetaryGameWorld::RenderPlanetaryBackground()
{
    if (!m_CurrentPlanet)
        return;

    // Clear with space-like background
    Drawing *render = m_GameWindow->Render();
    render->SetColor(5, 5, 20, 255); // Deep space color
    render->Clear();

    // Render the planet and its atmosphere
    m_CurrentPlanet->RenderPlanet(m_GameWindow->Renderer(),
                                  m_GameWindow->GetWidth(),
                                  m_GameWindow->GetHeight());
}

void PlanetaryGameWorld::TickCamera()
{
    TickPlanetaryCamera();
}

void PlanetaryGameWorld::TickBackground()
{
    if (!m_CurrentPlanet)
    {
        // Fallback to regular background if no planet
        GameWorld::TickBackground();
        return;
    }

    // Create space particles/debris instead of regular stars
    for (int i = 0; i < 2; i++) // Fewer particles for performance
    {
        // Spawn particles further out in space
        double angle = (rand() % 360) * M_PI / 180.0;
        double distance = m_CurrentPlanet->GetConfig().radius * 2.0 + (rand() % 1000);
        auto random_position = Vec2d(cos(angle) * distance, sin(angle) * distance);

        // Convert to screen space
        Drawing *render = m_GameWindow->Render();
        Vec2d screen_pos = PlanetaryUtils::WorldToScreen(random_position,
                                                         Vec2d(render->GetCameraX(), render->GetCameraY()),
                                                         m_GameWindow->GetWidth(),
                                                         m_GameWindow->GetHeight(),
                                                         render->GetZoom());

        auto duration = 2000.0 + (rand() % 1000); // Longer duration
        m_Stars.emplace_back(screen_pos, Vec2d(0.0, 0.0), duration);
    }

    // Update existing particles with planetary physics influence
    Vec2i MousePosition;
    SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
    auto MousePositionD = Vec2d(MousePosition.x, MousePosition.y);

    for (int i = m_Stars.size() - 1; i >= 0; --i)
    {
        auto &[position, velocity, duration] = m_Stars[i];

        // Apply mouse influence (less strong than original)
        auto direction = position - MousePositionD;
        double distance = direction.Length();
        if (distance > 0)
        {
            velocity += direction.Normalize() / distance * 0.05; // Reduced influence
        }

        // Add some orbital motion around the planet center
        Drawing *render = m_GameWindow->Render();
        Vec2d planet_screen_center = PlanetaryUtils::WorldToScreen(Vec2d(0, 0),
                                                                   Vec2d(render->GetCameraX(), render->GetCameraY()),
                                                                   m_GameWindow->GetWidth(),
                                                                   m_GameWindow->GetHeight(),
                                                                   render->GetZoom());

        Vec2d to_planet = planet_screen_center - position;
        double planet_distance = to_planet.Length();
        if (planet_distance > 0)
        {
            // Weak orbital influence
            Vec2d orbital_force = Vec2d(-to_planet.y, to_planet.x).Normalize() * 0.01;
            velocity += orbital_force;
        }

        velocity *= 0.995; // Slight drag

        position.x += velocity.x;
        position.y += velocity.y;

        // Wrap around screen edges
        if (position.x > m_GameWindow->GetWidth())
            position.x -= m_GameWindow->GetWidth();
        else if (position.x < 0)
            position.x += m_GameWindow->GetWidth();

        if (position.y > m_GameWindow->GetHeight())
            position.y -= m_GameWindow->GetHeight();
        else if (position.y < 0)
            position.y += m_GameWindow->GetHeight();

        duration -= 1;
        if (duration <= 0.0)
            m_Stars.erase(m_Stars.begin() + i);
    }
}

void PlanetaryGameWorld::Tick()
{
    if (m_Paused || m_GameOver)
        return;

    if (!m_ShowNames)
        m_ShowNamesVisibility *= 0.98;
    TickBackground();
    TickSpawner();
    TickEntities();
    TickDestroy();
    m_Particles->Tick();
    TickCamera();

    m_CurrentTick++;
}

void PlanetaryGameWorld::Draw()
{
    Drawing *render = m_GameWindow->Render();
    SDL_Renderer *renderer = m_GameWindow->Renderer();

    // Draw space particles/stars
    render->SetColor(200, 200, 255, 255); // Slightly blue-white for space
    for (int i = m_Stars.size() - 1; i >= 0; --i)
    {
        auto &[position, velocity, duration] = m_Stars[i];

        auto size = std::max(1, (int)(duration / 1000.0));
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                int draw_x = (int)(position.x - size / 2 + j);
                int draw_y = (int)(position.y - size / 2 + k);

                // Add some twinkling effect
                if ((m_CurrentTick + i) % 60 < 45)
                { // Twinkle pattern
                    SDL_RenderDrawPoint(renderer, draw_x, draw_y);
                }
            }
        }
    }

    // Render planetary background
    RenderPlanetaryBackground();

    // Stop drawing entities when game is over
    if (!m_GameOver)
    {
        // Don't render the regular background tile - planet replaces it

        m_Particles->Draw();

        // Draw all entities
        for (auto Current : m_FirstType)
            for (; Current; Current = Current->NextType())
                Current->Draw();

        // Draw tiles if they exist (for surface structures)
        if (m_Tiles)
        {
            m_Tiles->Draw();
        }
    }

    // Draw UI elements (score, etc.)
    Texture *ScoreTexture = m_ScoreText->RequestUpdate();
    int ScoreWidth = int(ScoreTexture->GetWidth() * 2.5);
    int ScoreHeight = int(ScoreTexture->GetHeight() * 2.5);
    SDL_Rect ScoreRect = {0, int(m_GameWindow->GetHeight() - ScoreHeight), ScoreWidth, ScoreHeight};
    if (!m_GameOver)
    {
        render->RenderTexture(ScoreTexture->SDLTexture(), nullptr, ScoreRect);
    }
    else
    {
        // Render death overlay for planetary mode
        render->SetColor(0, 0, 0, 200);
        SDL_Rect full = {0, 0, m_GameWindow->GetWidth(), m_GameWindow->GetHeight()};
        SDL_RenderFillRect(renderer, &full);

        int pw = (int)(m_GameWindow->GetWidth() * 0.6);
        int ph = (int)(m_GameWindow->GetHeight() * 0.6);
        int px = (m_GameWindow->GetWidth() - pw) / 2;
        int py = (m_GameWindow->GetHeight() - ph) / 2;
        m_DeathPanelRect = {px, py, pw, ph};

        render->SetColor(20, 20, 30, 230);
        SDL_RenderFillRect(renderer, &m_DeathPanelRect);

        // Title
        render->SetColor(220, 40, 40, 255);
        TextSurface titleTex(m_GameWindow->Assetz(), m_GameWindow->Assetz()->TextHandler()->GetMainFont(), "You Died", {220,40,40});
        Texture *tTex = titleTex.RequestUpdate();
        int tw = tTex->GetWidth() * 3;
        int th = tTex->GetHeight() * 3;
        SDL_Rect titleRect = {px + (pw - tw) / 2, py + 20, tw, th};
        render->RenderTexture(tTex->SDLTexture(), nullptr, titleRect);

        // Score and playtime
        render->SetColor(200,200,200,255);
        char buf[256];
        std::snprintf(buf, sizeof(buf), "Score: %u", m_Score);
        TextSurface scoreLine(m_GameWindow->Assetz(), m_GameWindow->Assetz()->TextHandler()->GetMainFont(), buf, {200,200,200});
        Texture *sLineTex = scoreLine.RequestUpdate();
        SDL_Rect sRect = {px + 40, py + 100, (int)(sLineTex->GetWidth()*2.0), (int)(sLineTex->GetHeight()*2.0)};
        render->RenderTexture(sLineTex->SDLTexture(), nullptr, sRect);

        double seconds = 0.0;
        if (m_GameWindow->Timer())
            seconds = (double)m_CurrentTick / std::max(1.0, (double)m_GameWindow->Timer()->GetFramerate());
        int mins = (int)seconds / 60;
        int secs = (int)seconds % 60;
        std::snprintf(buf, sizeof(buf), "Playtime: %02d:%02d", mins, secs);
        TextSurface timeLine(m_GameWindow->Assetz(), m_GameWindow->Assetz()->TextHandler()->GetMainFont(), buf, {200,200,200});
        Texture *tLineTex = timeLine.RequestUpdate();
        SDL_Rect tRect = {px + 40, py + 140, (int)(tLineTex->GetWidth()*2.0), (int)(tLineTex->GetHeight()*2.0)};
        render->RenderTexture(tLineTex->SDLTexture(), nullptr, tRect);

        // Back to Menu button
        int buttonWidth = 300;
        int buttonHeight = 80;
        int buttonX = px + (pw - buttonWidth) / 2;
        int buttonY = py + ph - buttonHeight - 40;
        m_DeathBackButtonRect = {buttonX, buttonY, buttonWidth, buttonHeight};

        if (m_DeathBackHover)
            render->SetColor(100,200,255,255);
        else
            render->SetColor(80,180,230,255);
        SDL_RenderFillRect(renderer, &m_DeathBackButtonRect);

        TextSurface backTextSurface(m_GameWindow->Assetz(), m_GameWindow->Assetz()->TextHandler()->GetMainFont(), "Back to Menu", {10,10,10});
        Texture *buttonTexture = backTextSurface.RequestUpdate();
        SDL_Rect buttonTextRect = {buttonX + (buttonWidth - (int)(buttonTexture->GetWidth()*1.5))/2,
                                   buttonY + (buttonHeight - (int)(buttonTexture->GetHeight()*1.5))/2,
                                   (int)(buttonTexture->GetWidth()*1.5), (int)(buttonTexture->GetHeight()*1.5)};
        render->RenderTexture(buttonTexture->SDLTexture(), nullptr, buttonTextRect);
    }
}