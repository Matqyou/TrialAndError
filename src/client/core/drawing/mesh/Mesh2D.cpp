//
// Created by Matq on 09/11/2025.
//

#include "Mesh2D.h"
#include <client/core/drawing/Drawing.h>

SDL_GPUVertexAttribute GPUVertex2D::vertex_attributes[4] = {
	{ .location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 0 }, // position: 2 float - 8
	{ .location = 1, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 8 },  // screen_position: 2 float - 8
	{ .location = 2, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = 16 }, // uv: 2 float - 8
	{ .location = 3, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM, .offset = 24 } // color: 4 byte - 4
};

Mesh2D::Mesh2D(Uint32 init_num_vertices, Uint32 init_num_indices)
{
	vertices = nullptr;
	indices = nullptr;
	num_vertices = 0;
	num_indices = 0;
	size_vertices = 0;
	size_indices = 0;
	fill_vertices_offset = 0;
	fill_indices_offset = 0;

	// shaders
	//  vertex
	translation = Vec2f(0, 0);
	rotation = 0.0f;

	//  fragment
	texture = nullptr;
	draw_as_circles = false;

	// sdl_gpu
	gpu_vertices = nullptr;
	gpu_indices = nullptr;
	transfer_vert_offset = 0;
	transfer_idx_offset = 0;

	shutdown_event_subscription = -1;

	Initialize(init_num_vertices, init_num_indices);
}

void Mesh2D::Initialize(Uint32 init_num_vertices, Uint32 init_num_indices)
{
	vertices = new GPUVertex2D[init_num_vertices];
	indices = new uint32_t[init_num_indices];
	num_vertices = init_num_vertices;
	num_indices = init_num_indices;
	size_vertices = init_num_vertices * sizeof(GPUVertex2D);
	size_indices = init_num_indices * sizeof(uint32_t);

	shutdown_event_subscription = Drawing.ShutDownEvent().Subscribe([this]() { this->Destroy(); });
}

void Mesh2D::Destroy()
{
	SDL_ReleaseGPUBuffer(Drawing.Device(), gpu_vertices);
	SDL_ReleaseGPUBuffer(Drawing.Device(), gpu_indices);
	gpu_vertices = nullptr;
	gpu_indices = nullptr;

	delete[] vertices;
	delete[] indices;
	vertices = nullptr;
	indices = nullptr;

	if (shutdown_event_subscription != -1)
		Drawing.ShutDownEvent().Unsubscribe(shutdown_event_subscription);
	shutdown_event_subscription = -1;
}

Mesh2D::~Mesh2D()
{
//	Destroy();
}

void Mesh2D::Draw() const
{
	Drawing.ShaderParams2D(translation, rotation, texture != nullptr, draw_as_circles);
	Drawing.DrawTriangles(gpu_vertices, gpu_indices, num_indices, texture);
}