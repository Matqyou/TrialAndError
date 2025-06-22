// PlanetaryGameWorld.h
#pragma once

#include <cmath>
#include <vector>
#include "SDL.h"
#include "shared/utility/Vec2.h"
#include "client/core/Assets.h"
#include "PlanetaryUtils.h"

// Forward declarations
class Character;
class GameWorld;
class GameData;

// Main planetary world system
class PlanetaryWorld
{
private:
    PlanetConfig m_Config;
    Texture *m_PlanetTexture;
    Texture *m_AtmosphereTexture;

    // Background layers for parallax effect
    std::vector<Texture *> m_BackgroundLayers;
    std::vector<double> m_LayerDistances;

    // Camera tracking
    Vec2d m_CameraPos;
    double m_CameraZoom;
    Vec2d m_LastPlayerPos;

    // Performance optimization
    int m_RenderSegments;
    double m_LastCameraAngle;

    // Internal rendering methods
    void RenderPlanetCore(SDL_Renderer *renderer, int screen_width, int screen_height);
    void RenderAtmosphere(SDL_Renderer *renderer, int screen_width, int screen_height);
    void RenderBackgroundLayers(SDL_Renderer *renderer, int screen_width, int screen_height);

public:
    PlanetaryWorld(const PlanetConfig &config = PlanetConfig());
    ~PlanetaryWorld();

    // Configuration
    void SetConfig(const PlanetConfig &config);
    const PlanetConfig &GetConfig() const { return m_Config; }

    // Getters
    double GetPlanetRadius() const { return m_Config.radius; }
    double GetAtmosphereHeight() const { return m_Config.atmosphere_height; }
    Vec2d GetCameraPos() const { return m_CameraPos; }
    double GetCameraZoom() const { return m_CameraZoom; }

    // Setters
    void SetPlanetRadius(double radius) { m_Config.radius = radius; }
    void SetCameraPos(const Vec2d &pos) { m_CameraPos = pos; }
    void SetCameraZoom(double zoom) { m_CameraZoom = zoom; }

    // Texture management
    bool LoadPlanetTexture(const char *texture_path);
    bool LoadAtmosphereTexture(const char *texture_path);
    void AddBackgroundLayer(const char *texture_path, double distance);

    // Physics calculations
    double GetSurfaceHeight(double angle) const; // For terrain variation

    Vec2d GetSurfaceNormal(const Vec2d &world_pos) const;

    // Rendering
    void RenderPlanet(SDL_Renderer *renderer, int screen_width, int screen_height);
    void UpdateCamera(const Vec2d &target_pos, double zoom_level = 1.0);

    // Utility
    PlanetaryCoords WorldToPlanetary(const Vec2d &world_pos) const;
    Vec2d PlanetaryToWorld(const PlanetaryCoords &coords) const;
    double GetDistanceFromSurface(const Vec2d &world_pos) const;
};

// Enhanced GameWorld with planetary support
class PlanetaryGameWorld : public GameWorld
{
private:
    PlanetaryWorld *m_CurrentPlanet;
    std::vector<PlanetConfig> m_AvailablePlanets;
    int m_CurrentPlanetIndex;
    double m_CamLonDeg = 0.0; // 0-360°
    double m_CamLatDeg = 0.0; // −90 … +90°
    // Planetary-specific rendering
    void TickPlanetaryCamera();
    void RenderPlanetaryBackground();

protected:
    // Override base methods
    void TickCamera() override;
    void TickBackground() override;

public:
    PlanetaryGameWorld(GameData *game_window, int width, int height);
    ~PlanetaryGameWorld();

    // Planet management
    void AddCameraAngles(double dLonDeg, double dLatDeg);
    void SetCurrentPlanet(int planet_index);
    void AddPlanetConfig(const PlanetConfig &config);
    PlanetaryWorld *GetCurrentPlanet() const { return m_CurrentPlanet; }

    // Level progression
    void NextPlanet();
    void PreviousPlanet();

    // Overridden methods
    void Draw() override;
    void Tick() override;
};
