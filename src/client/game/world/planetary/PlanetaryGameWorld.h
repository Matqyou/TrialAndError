// PlanetaryGameWorld.h
#pragma once

#include "PlanetaryUtils.h"
#include <client/game/world/GameWorld.h>
#include <client/core/Assets.h>

#include <shared/utility/Vec2.h>
#include <cmath>
#include <vector>

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
	std::vector<float> m_LayerDistances;

	double m_Yaw = 0.0;   // A/D
	double m_Pitch = 0.0; // W/S
	// Camera tracking
	Vec2f m_CameraPos;
	float m_CameraZoom;
	Vec2f m_LastPlayerPos;

	// Performance optimization
	int m_RenderSegments;
	double m_LastCameraAngle;

	// Internal rendering methods
	void RenderPlanetCore(SDL_Renderer *renderer, int screen_width, int screen_height);
	void RenderAtmosphere(SDL_Renderer *renderer, int screen_width, int screen_height);
	void RenderBackgroundLayers(SDL_Renderer *renderer, int screen_width, int screen_height);

public:
	explicit PlanetaryWorld(const PlanetConfig& config = PlanetConfig());
	~PlanetaryWorld();

	// Configuration
	void SetConfig(const PlanetConfig& config);

	// Getters
	[[nodiscard]] const PlanetConfig& GetConfig() const { return m_Config; }
	[[nodiscard]] float GetPlanetRadius() const { return m_Config.radius; }
	[[nodiscard]] float GetAtmosphereHeight() const { return m_Config.atmosphere_height; }
	[[nodiscard]] Vec2f GetCameraPos() const { return m_CameraPos; }
	[[nodiscard]] float GetCameraZoom() const { return m_CameraZoom; }

	// Setters
	void SetPlanetRadius(float radius) { m_Config.radius = radius; }
	void SetCameraPos(const Vec2f& pos) { m_CameraPos = pos; }
	void SetCameraZoom(float zoom) { m_CameraZoom = zoom; }

	// Texture management
	bool LoadPlanetTexture(const char *texture_path);
	bool LoadAtmosphereTexture(const char *texture_path);
	void AddBackgroundLayer(const char *texture_path, double distance);

	// Physics calculations
	[[nodiscard]] double GetSurfaceHeight(double angle) const; // For terrain variation
	[[nodiscard]] Vec2f GetSurfaceNormal(const Vec2f& world_pos) const;
	void AddAngles(double longChange, double latChange);

	// Rendering
	void RenderPlanet();
	void UpdateCamera(const Vec2f& target_pos, float zoom_level = 1.0f);

	// Utility
	PlanetaryCoords WorldToPlanetary(const Vec2f& world_pos) const;
	Vec2f PlanetaryToWorld(const PlanetaryCoords& coords) const;
	float GetDistanceFromSurface(const Vec2f& world_pos) const;
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
	void TickCamera(double seconds_elapsed) override;
	void TickBackground(double seconds_elapsed) override;

public:
	PlanetaryGameWorld(int width, int height);
	~PlanetaryGameWorld();

	// Planet management
	void SetCurrentPlanet(int planet_index);
	void AddPlanetConfig(const PlanetConfig& config);
	PlanetaryWorld *GetCurrentPlanet() const { return m_CurrentPlanet; }

	// Level progression
	void NextPlanet();
	void PreviousPlanet();

	// Overridden methods
	void Tick(double seconds_elapsed) override;
	void Draw() override;

};
