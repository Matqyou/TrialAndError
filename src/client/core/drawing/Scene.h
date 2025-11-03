//
// Created by Matq on 01/11/2025.
//

#pragma once

#include <shared/math/Vec2.h>
#include <shared/math/Vec3.h>

#include <SDL3/SDL.h>
#include <vector>

// Forward declarations
class Camera3D;
class Mesh;

struct DirectionLight
{
	Vec3f direction;
	Vec3f color;
	float ambient;

	DirectionLight(const Vec3f& start_direction, const Vec3f& start_color, float start_ambient);
	SDL_FColor GetShadedColor(const Vec3f& normal, SDL_FColor tri_color);
};

class Scene
{
private:
	std::vector<SDL_Vertex> vertices;
	std::vector<int> indices;

	struct Tri
	{
		Vec2f v0, v1, v2; // triangle positions on screen
		SDL_FColor color;
		Vec2f uv0, uv1, uv2;
		float depth; // drawing order
	};
	std::vector<Tri> triangle_buffer;

public:
	// Getting
	[[nodiscard]] std::vector<SDL_Vertex>& GetVertices() { return vertices; }
	[[nodiscard]] std::vector<int>& GetIndices() { return indices; }

	void Clear();
	void AddTriangle(Vec2f p0, Vec2f p1, Vec2f p2, SDL_FColor color, Vec2f uv0, Vec2f uv1, Vec2f uv2, float depth);
	void SortAndCommit();
	void ProjectMesh(const Mesh& mesh, const Camera3D& camera, DirectionLight* light);

};