//
// Created by Matq on 09/11/2025.
//

#include "Mesh.h"
#include <client/core/drawing/Drawing.h>

SDL_GPUVertexAttribute GPUVertex::vertex_attributes[4] = {
	{ .location = 0, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .offset = 0 },  // position
	{ .location = 1, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, .offset = sizeof(float) * 3 }, // normal
	{ .location = 2, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, .offset = sizeof(float) * 6 }, // UV
	{ .location = 3, .buffer_slot = 0, .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM, .offset = sizeof(float) * 8 } // color
};

Mesh::Mesh()
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
	translation = Mat4x4::Identity();

	//  fragment
	texture = nullptr;

	// sdl_gpu
	gpu_vertices = nullptr;
	gpu_indices = nullptr;
	transfer_vert_offset = 0;
	transfer_idx_offset = 0;

	shutdown_event_subscription = -1;
	is_copy = true;
}

Mesh::Mesh(Uint32 init_num_vertices, Uint32 init_num_indices)
	: Mesh()
{
	Initialize(init_num_vertices, init_num_indices);
}

void Mesh::Initialize(Uint32 init_num_vertices, Uint32 init_num_indices)
{
	vertices = new GPUVertex[init_num_vertices];
	indices = new uint32_t[init_num_indices];
	num_vertices = init_num_vertices;
	num_indices = init_num_indices;
	size_vertices = init_num_vertices * sizeof(GPUVertex);
	size_indices = init_num_indices * sizeof(uint32_t);

	shutdown_event_subscription = Drawing.ShutDownEvent().Subscribe([this]() { this->Destroy(); });
	is_copy = false;

	GPUVertex dummy_vertex = {
		.x = 0, .y = 0, .z = 0,
		.nx = 0, .ny = 0, .nz = 0,
		.u = 0, .v = 0,
		.color = { 255, 255, 255, 255 }
	};
	for (int i = 0; i < num_vertices; i++)
		vertices[i] = dummy_vertex;
	memset(indices, 0, size_indices); // safety precaution
}

void Mesh::Destroy()
{
	if (is_copy)
		return;

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

Mesh::~Mesh()
{
//	Destroy();
}

void Mesh::FlatNormals()
{
	for (int i = 0; i < num_indices; i += 3)
	{
		uint32_t i0 = indices[i];
		uint32_t i1 = indices[i + 1];
		uint32_t i2 = indices[i + 2];

		Vec3f v0 = Vec3f(vertices[i0].x, vertices[i0].y, vertices[i0].z);
		Vec3f v1 = Vec3f(vertices[i1].x, vertices[i1].y, vertices[i1].z);
		Vec3f v2 = Vec3f(vertices[i2].x, vertices[i2].y, vertices[i2].z);

		// Compute face normal
		Vec3f faceNormal = CrossProductVec3(v1 - v0, v2 - v0).NormalizeF();

		// Assign the same normal to all 3 vertices of this face
		vertices[i0].nx = faceNormal.x;
		vertices[i0].ny = faceNormal.y;
		vertices[i0].nz = faceNormal.z;

		vertices[i1].nx = faceNormal.x;
		vertices[i1].ny = faceNormal.y;
		vertices[i1].nz = faceNormal.z;

		vertices[i2].nx = faceNormal.x;
		vertices[i2].ny = faceNormal.y;
		vertices[i2].nz = faceNormal.z;
	}
}

void Mesh::SmoothNormals()
{
	for (int i = 0; i < num_vertices; i++)
	{
		vertices[i].nx = 0.0f;
		vertices[i].ny = 0.0f;
		vertices[i].nz = 0.0f;
	}
	for (int i = 0; i < num_indices; i += 3)
	{
		uint32_t i0 = indices[i];
		uint32_t i1 = indices[i + 1];
		uint32_t i2 = indices[i + 2];

		Vec3f v0 = Vec3f(vertices[i0].x, vertices[i0].y, vertices[i0].z);
		Vec3f v1 = Vec3f(vertices[i1].x, vertices[i1].y, vertices[i1].z);
		Vec3f v2 = Vec3f(vertices[i2].x, vertices[i2].y, vertices[i2].z);

		// Compute face normal
		Vec3f faceNormal = CrossProductVec3(v1 - v0, v2 - v0).NormalizeF();

		// Add to all 3 vertices
		vertices[i0].nx += faceNormal.x;
		vertices[i0].ny += faceNormal.y;
		vertices[i0].nz += faceNormal.z;
		vertices[i1].nx += faceNormal.x;
		vertices[i1].ny += faceNormal.y;
		vertices[i1].nz += faceNormal.z;
		vertices[i2].nx += faceNormal.x;
		vertices[i2].ny += faceNormal.y;
		vertices[i2].nz += faceNormal.z;
	}
	for (int i = 0; i < num_vertices; i++)
	{
		Vec3f normal = Vec3f(vertices[i].nx, vertices[i].ny, vertices[i].nz).NormalizeF();
		vertices[i].nx = normal.x;
		vertices[i].ny = normal.y;
		vertices[i].nz = normal.z;
	}
}

void Mesh::UpdateColor(const SDL_Color& new_color)
{
	for (int i = 0; i < num_vertices; i++)
		vertices[i].color = new_color;
}

void Mesh::Draw() const
{
	if (!gpu_vertices || !gpu_indices)
	{
		dbg_msg("Mesh::Draw() &cnullptr vertices or indices\n");
		return;
	}

	Drawing.ShaderParams(translation, texture != nullptr);
	Drawing.DrawTriangles(gpu_vertices, gpu_indices, num_indices, texture);
}