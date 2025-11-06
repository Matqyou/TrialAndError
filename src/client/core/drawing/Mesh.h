//
// Created by Matq on 01/11/2025.
//

#pragma once

#include <shared/geometry/Quaternion.h>
#include "shared/geometry/Geometry.h"
#include "shared/string/Strings.h"
#include "shared/math/Vec2.h"

#include "SDL3/SDL.h"
#include <vector>

struct Mesh
{
	std::vector<Vec3f> vertices;
	std::vector<Vec3f> face_normals;  // ← One per TRIANGLE, not vertex
	std::vector<int> indices;
	Vec3f position;
	SDL_FColor color;

	std::vector<Vec2f> uvs;
	SDL_GPUTexture *gpu_texture;

	Mesh()
	{
		color = { 255, 255, 255, 1.0f };
		gpu_texture = nullptr;
	}

	void PostProcessing()
	{
		face_normals.clear();

		// One normal per triangle
//		for (size_t i = 0; i < indices.size(); i += 3)
//		{
//			int idx0 = indices[i];
//			int idx1 = indices[i + 1];
//			int idx2 = indices[i + 2];
//
//			Vec3f v0 = vertices[idx0];
//			Vec3f v1 = vertices[idx1];
//			Vec3f v2 = vertices[idx2];
//
//			Vec3f face_normal = Geometry::TriangleNormal(v0, v1, v2);
//			face_normals.push_back(face_normal);
//		}

		for (int i = 0; i < indices.size(); i++)
			face_normals.push_back(Vec3f(0, 0, 0));
		for (int i = 0; i < indices.size(); i += 3)
		{
			int i0 = indices[i];
			int i1 = indices[i + 1];
			int i2 = indices[i + 2];

			Vec3f v0 = vertices[i0];
			Vec3f v1 = vertices[i1];
			Vec3f v2 = vertices[i2];

			// Compute face normal
			Vec3f faceNormal = CrossProductVec3(v1 - v0, v2 - v0).NormalizeF();

			// Add to all 3 vertices
			face_normals[i0] = face_normals[i0] + faceNormal;
			face_normals[i1] = face_normals[i1] + faceNormal;
			face_normals[i2] = face_normals[i2] + faceNormal;
		}

		// Normalize all vertex normals
		for (auto& n : face_normals)
			n = n.NormalizeF();


		if (uvs.empty())
		{
			for (size_t i = 0; i < indices.size(); i += 3)
			{
				uvs.emplace_back(0.0f, 0.0f);
				uvs.emplace_back(1.0f, 0.0f);
				uvs.emplace_back(0.0f, 1.0f);
			}
		}
	}
};

struct GPUMesh
{
	SDL_GPUBuffer *vertex_buffer;
	SDL_GPUBuffer *indice_buffer;
	uint32_t indice_count;
	SDL_GPUTexture *gpu_texture;

	Vec3f position;
	Quaternion orientation;

	GPUMesh()
	{
		vertex_buffer = nullptr;
		indice_buffer = nullptr;
		indice_count = 0;

		gpu_texture = nullptr;
		position = Vec3f(0, 0, 0);
		orientation = Quaternion::Identity();
	}
//	~GPUMesh()
//	{
//		SDL_ReleaseGPUBuffer()
//	}
};

namespace MeshPresets
{
inline Mesh CreateCube(float size)
{
	Mesh cube;
	float h = size / 2.0f;

	// 8 unique vertices
	cube.vertices = {
		Vec3f(-h, -h, -h),  // 0: back-bottom-left
		Vec3f(h, -h, -h),  // 1: back-bottom-right
		Vec3f(h, h, -h),  // 2: back-top-right
		Vec3f(-h, h, -h),  // 3: back-top-left
		Vec3f(-h, -h, h),  // 4: front-bottom-left
		Vec3f(h, -h, h),  // 5: front-bottom-right
		Vec3f(h, h, h),  // 6: front-top-right
		Vec3f(-h, h, h)   // 7: front-top-left
	};

	// 36 indices = 12 triangles * 3 vertices each
	cube.indices = {
		// Front face
		4, 5, 6, 4, 6, 7,
		// Back face
		1, 0, 3, 1, 3, 2,
		// Top face
		7, 6, 2, 7, 2, 3,
		// Bottom face
		0, 1, 5, 0, 5, 4,
		// Right face
		5, 1, 2, 5, 2, 6,
		// Left face
		0, 4, 7, 0, 7, 3
	};

	// UV coordinates - one per vertex (8 total)
	// Each front face will use these to map the full texture
	cube.uvs = {
		Vec2f(0.0f, 0.0f),  // 0: back-bottom-left
		Vec2f(1.0f, 0.0f),  // 1: back-bottom-right
		Vec2f(1.0f, 1.0f),  // 2: back-top-right
		Vec2f(0.0f, 1.0f),  // 3: back-top-left
		Vec2f(0.0f, 0.0f),  // 4: front-bottom-left
		Vec2f(1.0f, 0.0f),  // 5: front-bottom-right
		Vec2f(1.0f, 1.0f),  // 6: front-top-right
		Vec2f(0.0f, 1.0f)   // 7: front-top-left
	};

	cube.PostProcessing();

	return cube;
}

inline Mesh CreateSphere(float radius, int rings, int segments)
{
	Mesh sphere;

	// Generate vertices
	for (int ring = 0; ring <= rings; ++ring)
	{
		float phi = M_PI * ring / rings;
		phi = std::max(0.0f, std::min((float)M_PI, phi));

		for (int seg = 0; seg <= segments; ++seg)
		{
			float theta = 2.0f * M_PI * seg / segments;

			float x = sin(phi) * cos(theta);
			float y = cos(phi);
			float z = sin(phi) * sin(theta);

			sphere.vertices.push_back(Vec3f(
				x * radius,
				y * radius,
				z * radius
			));

			float u = (float)seg / segments;
			float v = (float)ring / rings;
			sphere.uvs.push_back(Vec2f(u, v));
		}
	}

	// Generate indices (two triangles per quad) - FIXED WINDING ORDER
	for (int ring = 0; ring < rings; ++ring)
	{
		for (int seg = 0; seg < segments; ++seg)
		{
			int current = ring * (segments + 1) + seg;
			int next = current + segments + 1;

			// First triangle (counter-clockwise from outside)
			sphere.indices.push_back(current);
			sphere.indices.push_back(current + 1);
			sphere.indices.push_back(next);

			// Second triangle (counter-clockwise from outside)
			sphere.indices.push_back(current + 1);
			sphere.indices.push_back(next + 1);
			sphere.indices.push_back(next);
		}
	}

	sphere.PostProcessing();

	return sphere;
}

inline Mesh CreateGround(float size, int subdivisions)
{
	Mesh plane;
	float step = size / subdivisions;
	float half = size / 2.0f;

	// Generate vertices in a grid on the XZ plane (Y=0)
	for (int z = 0; z <= subdivisions; ++z)
	{
		for (int x = 0; x <= subdivisions; ++x)
		{
			plane.vertices.push_back(Vec3f(
				-half + x * step,
				0.0f,  // Ground level at Y=0
				-half + z * step
			));
		}
	}

	// Generate indices (two triangles per quad)
	for (int z = 0; z < subdivisions; ++z)
	{
		for (int x = 0; x < subdivisions; ++x)
		{
			int topLeft = z * (subdivisions + 1) + x;
			int topRight = topLeft + 1;
			int bottomLeft = (z + 1) * (subdivisions + 1) + x;
			int bottomRight = bottomLeft + 1;

			// First triangle (counter-clockwise when viewed from above)
			plane.indices.push_back(topLeft);
			plane.indices.push_back(bottomLeft);
			plane.indices.push_back(topRight);

			// Second triangle
			plane.indices.push_back(topRight);
			plane.indices.push_back(bottomLeft);
			plane.indices.push_back(bottomRight);
		}
	}

	plane.PostProcessing();
	return plane;
}
}