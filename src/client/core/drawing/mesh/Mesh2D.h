//
// Created by Matq on 09/11/2025.
//

#pragma once

#include <shared/string/Strings.h>
#include <shared/math/Vec2.h>
#include <SDL3/SDL.h>

struct GPUVertex2D
{
	float x, y;
	float sx, sy;
	float u, v;
	SDL_Color color;

	static SDL_GPUVertexAttribute vertex_attributes[4];
};

struct Mesh2D
{
	GPUVertex2D *vertices;
	uint32_t *indices;
	Uint32 num_vertices;
	Uint32 num_indices;
	Uint32 size_vertices;
	Uint32 size_indices;

	Uint32 fill_vertices_offset;
	Uint32 fill_indices_offset;

	// shaders
	//  vertex
	Vec2f translation;
	float rotation;

	//  fragment
	SDL_GPUTexture *texture;
	bool draw_as_circles;

	// sdl_gpu
	SDL_GPUBuffer *gpu_vertices;
	SDL_GPUBuffer *gpu_indices;
	Uint32 transfer_vert_offset;
	Uint32 transfer_idx_offset;

	int shutdown_event_subscription;

	Mesh2D(Uint32 init_num_vertices, Uint32 init_num_indices);
	void Initialize(Uint32 init_num_vertices, Uint32 init_num_indices);
	void Destroy();
	~Mesh2D();

	// Options
	Mesh2D& SetTranslation(const Vec2f& new_translation)
	{
		translation = new_translation;
		return *this;
	}
	Mesh2D& SetRotation(float new_rotation)
	{
		rotation = new_rotation;
		return *this;
	}
	//
	Mesh2D& SetTexture(SDL_GPUTexture *new_texture)
	{
		texture = new_texture;
		return *this;
	}
	Mesh2D& SetDrawAsCircles(bool new_draw_as_circles)
	{
		draw_as_circles = new_draw_as_circles;
		return *this;
	}

	GPUVertex2D *AllocateVertices(Uint32 required_vertices)
	{
		Uint32 original_offset = fill_vertices_offset;
		fill_vertices_offset += required_vertices;
		if (fill_vertices_offset > num_vertices)
			throw std::runtime_error(Strings::FString("Mesh2D::AllocateVertices() Allocated vertices exceeds number of vertices"));

		return &vertices[original_offset];
	}
	uint32_t *AllocateIndices(Uint32 required_indices)
	{
		Uint32 original_offset = fill_indices_offset;
		fill_indices_offset += required_indices;
		if (fill_indices_offset > num_indices)
			throw std::runtime_error(Strings::FString("Mesh2D::AllocateIndices() Allocated indices exceeds number of indices"));

		return &indices[original_offset];
	}

	// Ticking
	void Draw() const;
};