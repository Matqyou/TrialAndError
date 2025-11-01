// PlanetarySystem.cpp

#include "PlanetaryUtils.h"
#include <algorithm>
#include <random>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Planetary coordinate system
PlanetaryCoords::PlanetaryCoords(float lon, float lat, float r)
	: longitude(lon), latitude(lat), planet_radius(r)
{

}

Vec2f PlanetaryCoords::ToCartesian() const
{
	// longitude and latitude are stored in radians
	float lonRad = longitude;
	float latRad = latitude;

	float x = planet_radius * cosf(latRad) * cosf(lonRad);
	float y = planet_radius * sinf(latRad);
	return { x, y };
}

PlanetaryCoords PlanetaryCoords::FromCartesian(const Vec2f& pos, float r)
{
	float lonRad = pos.Atan2F();
	float hyp = pos.LengthF();
	float latRad = 0.0f;
	if (hyp > 0.0f)
		latRad = asinf(std::clamp(pos.y / hyp, -1.0f, 1.0f));

	PlanetaryCoords pc;
	pc.planet_radius = r;
	// store in radians, normalize longitude to [0, 2PI)
	pc.longitude = fmodf(lonRad + 2.0f * static_cast<float>(M_PI), 2.0f * static_cast<float>(M_PI));
	pc.latitude = std::clamp(latRad, static_cast<float>(-M_PI) / 2.0f, static_cast<float>(M_PI) / 2.0f);
	return pc;
}

void PlanetaryCoords::NormalizeAngle()
{
	longitude = fmodf(longitude + 2.0f * static_cast<float>(M_PI), 2.0f * static_cast<float>(M_PI));
	latitude = std::clamp(latitude, static_cast<float>(-M_PI) / 2.0f, static_cast<float>(M_PI) / 2.0f);
}

namespace PlanetaryUtils
{

// Convert screen coordinates to world coordinates
Vec2d ScreenToWorld(const Vec2d& screen_pos, const Vec2d& camera_pos,
					int screen_width, int screen_height, double zoom)
{
	return {
		(screen_pos.x - screen_width / 2.0) / zoom + camera_pos.x,
		(screen_pos.y - screen_height / 2.0) / zoom + camera_pos.y };
}

// Convert world coordinates to screen coordinates
Vec2d WorldToScreen(const Vec2f& world_pos, const Vec2f& camera_pos,
					int screen_width, int screen_height, double zoom)
{
	return {
		(world_pos.x - camera_pos.x) * zoom + screen_width / 2.0,
		(world_pos.y - camera_pos.y) * zoom + screen_height / 2.0
	};
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
