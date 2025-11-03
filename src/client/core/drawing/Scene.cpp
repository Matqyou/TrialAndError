//
// Created by Matq on 01/11/2025.
//

#include "Scene.h"
#include <client/core/drawing/Camera3D.h>
#include <client/core/drawing/Mesh.h>
#include <client/core/Application.h>

#include <algorithm>

// ========== DirectionLight Implementation ==========

DirectionLight::DirectionLight(const Vec3f& start_direction, const Vec3f& start_color, float start_ambient)
{
	direction = start_direction;
	color = start_color;
	ambient = start_ambient;
}

SDL_FColor DirectionLight::GetShadedColor(const Vec3f& normal, SDL_FColor tri_color)
{
	// Normalize the normal (should already be normalized, but just in case)
	Vec3f n = normal.NormalizeF();

	// Diffuse lighting (Lambertian)
	float diffuse = std::max(0.0f, DotProductVec3(n, direction));

	// Combine ambient + diffuse
	float intensity = ambient + (1.0f - ambient) * diffuse;
	intensity = std::clamp(intensity, 0.0f, 1.0f);

	// Normalize light color from 0-255 to 0-1 range
	float light_r = color.x / 255.0f;
	float light_g = color.y / 255.0f;
	float light_b = color.z / 255.0f;

	// Apply light color AND intensity to base color
	return {
		tri_color.r * intensity * light_r,
		tri_color.g * intensity * light_g,
		tri_color.b * intensity * light_b,
		tri_color.a
	};
}

// ========== Scene Implementation ==========

void Scene::Clear()
{
	vertices.clear();
	indices.clear();
	triangle_buffer.clear();
}

void Scene::AddTriangle(Vec2f p0, Vec2f p1, Vec2f p2, SDL_FColor color, Vec2f uv0, Vec2f uv1, Vec2f uv2, float depth)
{
	triangle_buffer.push_back({ p0, p1, p2, color, uv0, uv1, uv2, depth });
}

void Scene::SortAndCommit()
{
	// drawing order
	std::sort(triangle_buffer.begin(), triangle_buffer.end(),
			  [](const Tri& a, const Tri& b)
			  {
				  return a.depth > b.depth; // Draw far triangles first
			  });

	// convert triangles into screen vertices, indices
	for (const auto& tri : triangle_buffer)
	{
		int base_index = static_cast<int>(vertices.size());

		vertices.push_back({{ tri.v0.x, tri.v0.y }, tri.color, { tri.uv0.x, tri.uv0.y }});
		vertices.push_back({{ tri.v1.x, tri.v1.y }, tri.color, { tri.uv1.x, tri.uv1.y }});
		vertices.push_back({{ tri.v2.x, tri.v2.y }, tri.color, { tri.uv2.x, tri.uv2.y }});

		indices.push_back(base_index);
		indices.push_back(base_index + 1);
		indices.push_back(base_index + 2);
	}

	triangle_buffer.clear();
}

void Scene::ProjectMesh(const Mesh& mesh, const Camera3D& camera, DirectionLight* light)
{
	for (size_t i = 0; i < mesh.indices.size(); i += 3)
	{
		int idx0 = mesh.indices[i];
		int idx1 = mesh.indices[i + 1];
		int idx2 = mesh.indices[i + 2];

		Vec3f worldV0 = mesh.vertices[idx0] + mesh.position;
		Vec3f worldV1 = mesh.vertices[idx1] + mesh.position;
		Vec3f worldV2 = mesh.vertices[idx2] + mesh.position;

		// BACKFACE CULLING: Use precomputed face normal
		Vec3f face_normal = mesh.face_normals[i / 3];
		Vec3f to_camera = (camera.GetPosition() - worldV0).NormalizeF();
		float facing = DotProductVec3(face_normal, to_camera);

		if (facing <= 0.0f)
			continue; // Skip back-facing triangles

		bool visible0, visible1, visible2;
		Vec2f s0 = camera.CameraToScreen(worldV0, visible0, Vec2f(Application.GetResolution()));
		Vec2f s1 = camera.CameraToScreen(worldV1, visible1, Vec2f(Application.GetResolution()));
		Vec2f s2 = camera.CameraToScreen(worldV2, visible2, Vec2f(Application.GetResolution()));

//		dbg_msg("Point: %.1fx %.1fy\n", s0.x, s0.y);

		if (visible0 || visible1 || visible2)
		{
			SDL_FColor color = mesh.color;
			if (light)
				color = light->GetShadedColor(face_normal, color);

			// Calculate depth along camera's look direction (view space depth)
			Vec3f camV0 = worldV0 - camera.GetPosition();
			Vec3f camV1 = worldV1 - camera.GetPosition();
			Vec3f camV2 = worldV2 - camera.GetPosition();

			// Project onto camera's forward vector to get true depth
			Vec3f look = camera.GetLook();
			float depth0 = DotProductVec3(camV0, look);
			float depth1 = DotProductVec3(camV1, look);
			float depth2 = DotProductVec3(camV2, look);

			float avg_depth = (depth0 + depth1 + depth2) / 3.0f;

			Vec2f uv0 = mesh.uvs[idx0];
			Vec2f uv1 = mesh.uvs[idx1];
			Vec2f uv2 = mesh.uvs[idx2];
			AddTriangle(s0, s1, s2, color, uv0, uv1, uv2, avg_depth);
		}
	}
}