// PlanetaryUtils.h
#pragma once

#include <cmath>
#include <vector>
#include "SDL.h"
#include <shared/utility/Vec2.h>
#include <client/core/Assets.h>
struct PlanetaryCoords {
    double longitude;      // Radians: 0–2PI (wraps)
    double latitude;       // Radians: -PI/2 to +PI/2 (clamped)
    double planet_radius;  // Radius of the planet (e.g., 1000 units)

    PlanetaryCoords(double lon = 0.0, double lat = 0.0, double r = 1000.0);

    // Convert to cartesian XY (on a sphere) for rendering/physics
    Vec2d ToCartesian() const;

    // Convert from cartesian position to planetary coordinates
    static PlanetaryCoords FromCartesian(const Vec2d& pos, double planet_radius);

    // Normalize longitude to [0, 2*PI), clamp latitude to [-PI/2, PI/2]
    void NormalizeAngle();
};

// Planet configuration for different levels
struct PlanetConfig {
    double radius;
    double atmosphere_height;
    SDL_Color surface_color;
    SDL_Color atmosphere_color;
    const char* texture_path;
    const char* name;
    
    PlanetConfig(double r = 2000.0, double atm = 200.0,
                SDL_Color surf = {100, 100, 100, 255}, 
                SDL_Color atm_col = {135, 206, 235, 100},
                const char* tex = nullptr, const char* planet_name = "Unknown Planet")
        : radius(r), atmosphere_height(atm),
          surface_color(surf), atmosphere_color(atm_col), 
          texture_path(tex), name(planet_name) {}
};


// Predefined planet configurations
namespace PlanetPresets {
    extern const PlanetConfig EARTH_LIKE;
    extern const PlanetConfig MARS_LIKE;
    extern const PlanetConfig JUPITER_LIKE;
    extern const PlanetConfig MOON_LIKE;
    extern const PlanetConfig ASTEROID;
    extern const PlanetConfig GAS_GIANT;
}

// Utility functions
namespace PlanetaryUtils {
    // Convert screen coordinates to world coordinates
    Vec2d ScreenToWorld(const Vec2d& screen_pos, const Vec2d& camera_pos, 
                       int screen_width, int screen_height, double zoom = 1.0);
    
    // Convert world coordinates to screen coordinates  
    Vec2d WorldToScreen(const Vec2d& world_pos, const Vec2d& camera_pos,
                       int screen_width, int screen_height, double zoom = 1.0);
    
    // Calculate optimal camera distance for planet
    double GetOptimalCameraDistance(double planet_radius, int screen_width, int screen_height);
    
    // Generate procedural planet surface
    std::vector<double> GeneratePlanetSurface(double planet_radius, int segments, 
                                            double roughness = 0.1, int seed = 0);
}
