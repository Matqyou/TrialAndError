//
// Created by Matq on 01/11/2025.
//

#pragma once

#include <shared/geometry/Quaternion.h>
#include <shared/geometry/Matrix4x4.h>
#include <shared/geometry/Geometry.h>
#include <shared/string/Strings.h>

#include <shared/math/Vec2.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL.h>
#include <vector>

struct GPUVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	SDL_Color color;

	static SDL_GPUVertexAttribute vertex_attributes[4];
};

struct Mesh
{
	GPUVertex *vertices;
	uint32_t *indices;
	Uint32 num_vertices;
	Uint32 num_indices;
	Uint32 size_vertices;
	Uint32 size_indices;

	Uint32 fill_vertices_offset;
	Uint32 fill_indices_offset;

	// shaders
	//  vertex
	Mat4x4 translation;

	//  fragment
	SDL_GPUTexture *texture;

	// sdl_gpu
	SDL_GPUBuffer *gpu_vertices;
	SDL_GPUBuffer *gpu_indices;
	Uint32 transfer_vert_offset;
	Uint32 transfer_idx_offset;

	bool is_copy;
	int shutdown_event_subscription;

	Mesh();
	Mesh(Uint32 init_num_vertices, Uint32 init_num_indices);
	void Initialize(Uint32 init_num_vertices, Uint32 init_num_indices);
	void Destroy();
	~Mesh();

	// Options
	Mesh& SetTranslation(const Mat4x4& new_translation)
	{
		translation = new_translation;
		return *this;
	}
	//
	Mesh& SetTexture(SDL_GPUTexture *new_texture)
	{
		texture = new_texture;
		return *this;
	}
	//
	Mesh& FlagCopy()
	{
		is_copy = true;
		return *this;
	}

	GPUVertex *AllocateVertices(Uint32 required_vertices)
	{
		Uint32 original_offset = fill_vertices_offset;
		fill_vertices_offset += required_vertices;
		if (fill_vertices_offset > num_vertices)
			throw std::runtime_error(Strings::FString("Mesh::AllocateVertices() Allocated vertices exceeds number of vertices"));

		return &vertices[original_offset];
	}
	uint32_t *AllocateIndices(Uint32 required_indices)
	{
		Uint32 original_offset = fill_indices_offset;
		fill_indices_offset += required_indices;
		if (fill_indices_offset > num_indices)
			throw std::runtime_error(Strings::FString("Mesh::AllocateIndices() Allocated indices exceeds number of indices"));

		return &indices[original_offset];
	}

	// Manipulating
	void FlatNormals();
	void SmoothNormals();
	void UpdateColor(const SDL_Color& new_color);

	// Ticking
	void Draw() const;
};

//struct Mesh
//{
//	std::vector<Vec3f> vertices;
//	std::vector<Vec3f> face_normals;  // ← One per TRIANGLE, not vertex
//	std::vector<int> indices;
//	Vec3f position;
//	SDL_FColor color;
//
//	std::vector<Vec2f> uvs;
//	SDL_GPUTexture *gpu_texture;
//
//	Mesh()
//	{
//		color = { 255, 255, 255, 1.0f };
//		gpu_texture = nullptr;
//	}
//
//	void PostProcessing()
//	{
//		face_normals.clear();
//
//		// One normal per triangle
////		for (size_t i = 0; i < indices.size(); i += 3)
////		{
////			int idx0 = indices[i];
////			int idx1 = indices[i + 1];
////			int idx2 = indices[i + 2];
////
////			Vec3f v0 = vertices[idx0];
////			Vec3f v1 = vertices[idx1];
////			Vec3f v2 = vertices[idx2];
////
////			Vec3f face_normal = Geometry::TriangleNormal(v0, v1, v2);
////			face_normals.push_back(face_normal);
////		}
//
//		for (int i = 0; i < indices.size(); i++)
//			face_normals.push_back(Vec3f(0, 0, 0));
//		for (int i = 0; i < indices.size(); i += 3)
//		{
//			int i0 = indices[i];
//			int i1 = indices[i + 1];
//			int i2 = indices[i + 2];
//
//			Vec3f v0 = vertices[i0];
//			Vec3f v1 = vertices[i1];
//			Vec3f v2 = vertices[i2];
//
//			// Compute face normal
//			Vec3f faceNormal = CrossProductVec3(v1 - v0, v2 - v0).NormalizeF();
//
//			// Add to all 3 vertices
//			face_normals[i0] = face_normals[i0] + faceNormal;
//			face_normals[i1] = face_normals[i1] + faceNormal;
//			face_normals[i2] = face_normals[i2] + faceNormal;
//		}
//
//		// Normalize all vertex normals
//		for (auto& n : face_normals)
//			n = n.NormalizeF();
//
//		if (uvs.empty())
//		{
//			for (size_t i = 0; i < indices.size(); i += 3)
//			{
//				uvs.emplace_back(0.0f, 0.0f);
//				uvs.emplace_back(1.0f, 0.0f);
//				uvs.emplace_back(0.0f, 1.0f);
//			}
//		}
//	}
//};
//
//struct GPUMesh
//{
//	SDL_GPUBuffer *vertex_buffer;
//	SDL_GPUBuffer *indice_buffer;
//	uint32_t indice_count;
//	SDL_GPUTexture *gpu_texture;
//
//	Vec3f position;
//	Quaternion orientation;
//
//	GPUMesh()
//	{
//		vertex_buffer = nullptr;
//		indice_buffer = nullptr;
//		indice_count = 0;
//
//		gpu_texture = nullptr;
//		position = Vec3f(0, 0, 0);
//		orientation = Quaternion::Identity();
//	}
////	~GPUMesh()
////	{
////		SDL_ReleaseGPUBuffer()
////	}
//};

namespace MeshPresets
{
inline Mesh CreateCube(float size)
{
	Mesh cube(8, 36);
	auto vertices = cube.AllocateVertices(8);
	auto indices = cube.AllocateIndices(36);
	float h = size / 2.0f;

	vertices[0] = { .x = -h, .y = -h, .z = -h, .u = 0.0f, .v = 0.0f }; // 0: back-bottom-left
	vertices[1] = { .x = h, .y = -h, .z = -h, .u = 1.0f, .v = 0.0f, }; // 1: back-bottom-right
	vertices[2] = { .x = h, .y = h, .z = -h, .u = 1.0f, .v = 1.0f, }; // 2: back-top-right
	vertices[3] = { .x = -h, .y = h, .z = -h, .u = 0.0f, .v = 1.0f, }; // 3: back-top-left
	vertices[4] = { .x = -h, .y = -h, .z = h, .u = 0.0f, .v = 0.0f, }; // 4: front-bottom-left
	vertices[5] = { .x = h, .y = -h, .z = h, .u = 1.0f, .v = 0.0f, }; // 5: front-bottom-right
	vertices[6] = { .x = h, .y = h, .z = h, .u = 1.0f, .v = 1.0f, }; // 6: front-top-right
	vertices[7] = { .x = -h, .y = h, .z = h, .u = 0.0f, .v = 1.0f, }; // 7: front-top-left

	uint32_t indices_copy[36] = {
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
	memcpy(indices, indices_copy, sizeof(indices_copy));

	cube.FlatNormals();
	cube.UpdateColor({ 255, 255, 255, 255 });
	return cube;
}

inline Mesh CreateTexturedCube(float size)
{
	Mesh cube(24, 36);  // 24 vertices (4 per face), 36 indices
	auto vertices = cube.AllocateVertices(24);
	auto indices = cube.AllocateIndices(36);
	float h = size / 2.0f;

	// Front face (indices 0-3)
	vertices[0] = { .x = -h, .y = -h, .z = h, .u = 1.0f / 3.0f, .v = 0.5f };
	vertices[1] = { .x = h, .y = -h, .z = h, .u = 0.0f / 3.0f, .v = 0.5f };
	vertices[2] = { .x = h, .y = h, .z = h, .u = 0.0f / 3.0f, .v = 0.0f };
	vertices[3] = { .x = -h, .y = h, .z = h, .u = 1.0f / 3.0f, .v = 0.0f };

	// Back face (indices 4-7)
	vertices[4] = { .x = h, .y = -h, .z = -h, .u = 2.0f / 3.0f, .v = 0.5f };
	vertices[5] = { .x = -h, .y = -h, .z = -h, .u = 1.0f / 3.0f, .v = 0.5f };
	vertices[6] = { .x = -h, .y = h, .z = -h, .u = 1.0f / 3.0f, .v = 0.0f };
	vertices[7] = { .x = h, .y = h, .z = -h, .u = 2.0f / 3.0f, .v = 0.0f };

	// Top face (indices 8-11)
	vertices[8] = { .x = -h, .y = h, .z = h, .u = 3.0f / 3.0f, .v = 0.5f };
	vertices[9] = { .x = h, .y = h, .z = h, .u = 2.0f / 3.0f, .v = 0.5f };
	vertices[10] = { .x = h, .y = h, .z = -h, .u = 2.0f / 3.0f, .v = 0.0f };
	vertices[11] = { .x = -h, .y = h, .z = -h, .u = 3.0f / 3.0f, .v = 0.0f };

	// Bottom face (indices 12-15)
	vertices[12] = { .x = -h, .y = -h, .z = -h, .u = 3.0f / 3.0f, .v = 1.0f };
	vertices[13] = { .x = h, .y = -h, .z = -h, .u = 2.0f / 3.0f, .v = 1.0f };
	vertices[14] = { .x = h, .y = -h, .z = h, .u = 2.0f / 3.0f, .v = 0.5f };
	vertices[15] = { .x = -h, .y = -h, .z = h, .u = 3.0f / 3.0f, .v = 0.5f };

	// Right face (indices 16-19)
	vertices[16] = { .x = h, .y = -h, .z = h, .u = 2.0f / 3.0f, .v = 1.0f };
	vertices[17] = { .x = h, .y = -h, .z = -h, .u = 1.0f / 3.0f, .v = 1.0f };
	vertices[18] = { .x = h, .y = h, .z = -h, .u = 1.0f / 3.0f, .v = 0.5f };
	vertices[19] = { .x = h, .y = h, .z = h, .u = 2.0f / 3.0f, .v = 0.5f };

	// Left face (indices 20-23)
	vertices[20] = { .x = -h, .y = -h, .z = -h, .u = 1.0f / 3.0f, .v = 1.0f };
	vertices[21] = { .x = -h, .y = -h, .z = h, .u = 0.0f / 3.0f, .v = 1.0f };
	vertices[22] = { .x = -h, .y = h, .z = h, .u = 0.0f / 3.0f, .v = 0.5f };
	vertices[23] = { .x = -h, .y = h, .z = -h, .u = 1.0f / 3.0f, .v = 0.5f };

	uint32_t indices_copy[36] = {
		// Front face
		0, 1, 2, 0, 2, 3,
		// Back face
		4, 5, 6, 4, 6, 7,
		// Top face
		8, 9, 10, 8, 10, 11,
		// Bottom face
		12, 13, 14, 12, 14, 15,
		// Right face
		16, 17, 18, 16, 18, 19,
		// Left face
		20, 21, 22, 20, 22, 23
	};
	memcpy(indices, indices_copy, sizeof(indices_copy));

	cube.FlatNormals();
	cube.UpdateColor({ 255, 255, 255, 255 });
	return cube;
}

inline Mesh CreateSphere(float radius, int rings, int segments)
{
	int num_vertices = (rings + 1) * (segments + 1);
	int num_indices = rings * segments * 6;

	Mesh sphere(num_vertices, num_indices);
	auto vertices = sphere.AllocateVertices(num_vertices);
	auto indices = sphere.AllocateIndices(num_indices);

	int vertex_index = 0;

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

			float u = (float)seg / segments;
			float v = (float)ring / rings;

			vertices[vertex_index] = {
				.x = x * radius,
				.y = y * radius,
				.z = z * radius,
				.u = u,
				.v = v
			};

			vertex_index++;
		}
	}

	// Generate indices (two triangles per quad)
	int index_offset = 0;
	for (int ring = 0; ring < rings; ++ring)
	{
		for (int seg = 0; seg < segments; ++seg)
		{
			int current = ring * (segments + 1) + seg;
			int next = current + segments + 1;

			// First triangle (counter-clockwise from outside)
			indices[index_offset++] = current;
			indices[index_offset++] = current + 1;
			indices[index_offset++] = next;

			// Second triangle (counter-clockwise from outside)
			indices[index_offset++] = current + 1;
			indices[index_offset++] = next + 1;
			indices[index_offset++] = next;
		}
	}

	sphere.FlatNormals();
	sphere.UpdateColor({ 255, 255, 255, 255 });
	return sphere;
}

inline Mesh CreateGround(float size, int subdivisions)
{
	int num_vertices = (subdivisions + 1) * (subdivisions + 1);
	int num_indices = subdivisions * subdivisions * 6;

	Mesh plane(num_vertices, num_indices);
	auto vertices = plane.AllocateVertices(num_vertices);
	auto indices = plane.AllocateIndices(num_indices);

	float step = size / subdivisions;
	float half = size / 2.0f;
	int vertex_index = 0;

	// Generate vertices in a grid on the XZ plane (Y=0)
	for (int z = 0; z <= subdivisions; ++z)
	{
		for (int x = 0; x <= subdivisions; ++x)
		{
			vertices[vertex_index] = {
				.x = -half + x * step,
				.y = 0.0f,  // Ground level at Y=0
				.z = -half + z * step,
				.u = (float)x / subdivisions,
				.v = (float)z / subdivisions
			};
			vertex_index++;
		}
	}

	// Generate indices (two triangles per quad)
	int index_offset = 0;
	for (int z = 0; z < subdivisions; ++z)
	{
		for (int x = 0; x < subdivisions; ++x)
		{
			int topLeft = z * (subdivisions + 1) + x;
			int topRight = topLeft + 1;
			int bottomLeft = (z + 1) * (subdivisions + 1) + x;
			int bottomRight = bottomLeft + 1;

			// First triangle (counter-clockwise when viewed from above)
			indices[index_offset++] = topLeft;
			indices[index_offset++] = bottomLeft;
			indices[index_offset++] = topRight;

			// Second triangle
			indices[index_offset++] = topRight;
			indices[index_offset++] = bottomLeft;
			indices[index_offset++] = bottomRight;
		}
	}

	plane.FlatNormals();
	plane.UpdateColor({ 255, 255, 255, 255 });
	return plane;
}
}