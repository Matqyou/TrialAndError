// PlanetarySystem.cpp
#include "client/GameData.h"
#include <random>
#include "PlanetaryUtils.h"


// Planetary coordinate system

PlanetaryCoords::PlanetaryCoords(double lon, double lat, double r)
    : longitude(lon), latitude(lat), planet_radius(r)
{
}

Vec2d PlanetaryCoords::ToCartesian() const
{
    // longitude and latitude are stored in radians
    double lonRad = longitude;
    double latRad = latitude;

    double x = planet_radius * cos(latRad) * cos(lonRad);
    double y = planet_radius * sin(latRad);
    return Vec2d(x, y);
}

PlanetaryCoords PlanetaryCoords::FromCartesian(const Vec2d& pos, double r)
{
    double lonRad = atan2(pos.y, pos.x);
    double hyp = sqrt(pos.x * pos.x + pos.y * pos.y);
    double latRad = 0.0;
    if (hyp > 0.0)
        latRad = asin(std::clamp(pos.y / hyp, -1.0, 1.0));

    PlanetaryCoords pc;
    pc.planet_radius = r;
    // store in radians, normalize longitude to [0, 2PI)
    pc.longitude = fmod(lonRad + 2.0 * M_PI, 2.0 * M_PI);
    pc.latitude  = std::clamp(latRad, -M_PI / 2.0, M_PI / 2.0);
    return pc;
}

void PlanetaryCoords::NormalizeAngle()
{
    longitude = fmod(longitude + 2.0 * M_PI, 2.0 * M_PI);
    latitude  = std::clamp(latitude, -M_PI / 2.0, M_PI / 2.0);
}

namespace PlanetaryUtils
{

    // Convert screen coordinates to world coordinates
    Vec2d ScreenToWorld(const Vec2d &screen_pos, const Vec2d &camera_pos,
                        int screen_width, int screen_height, double zoom)
    {
        return {
            (screen_pos.x - screen_width / 2.0) / zoom + camera_pos.x,
            (screen_pos.y - screen_height / 2.0) / zoom + camera_pos.y};
    }

    // Convert world coordinates to screen coordinates
    Vec2d WorldToScreen(const Vec2d &world_pos, const Vec2d &camera_pos,
                        int screen_width, int screen_height, double zoom)
    {
        return {
            (world_pos.x - camera_pos.x) * zoom + screen_width / 2.0,
            (world_pos.y - camera_pos.y) * zoom + screen_height / 2.0};
    }

    // Calculate optimal camera distance for showing the planet
    double GetOptimalCameraDistance(double planet_radius, int screen_width, int screen_height)
    {
        // Estimate the closest distance at which the entire planet fits on screen
        double screen_half_min = std::min(screen_width, screen_height) / 2.0;
        return planet_radius + screen_half_min; // Ensure entire radius is visible
    }

    // Generate a noisy circular surface (useful for procedural planets)
    std::vector<double> GeneratePlanetSurface(double planet_radius, int segments,
                                              double roughness, int seed)
    {
        std::vector<double> surface(segments, planet_radius);

        if (roughness <= 0.0)
            return surface;

        std::mt19937 rng(seed);
        std::uniform_real_distribution<double> dist(-roughness * planet_radius, roughness * planet_radius);

        for (int i = 0; i < segments; ++i)
        {
            surface[i] += dist(rng);
        }

        return surface;
    }
}