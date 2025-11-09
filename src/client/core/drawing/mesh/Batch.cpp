//
// Created by Matq on 09/11/2025.
//

#include "Batch.h"
#include <client/core/drawing/Drawing.h>

Batch::~Batch()
{
	for (Mesh *mesh : mesh_updates)
		delete mesh;
	for (Mesh2D *mesh_2d : mesh_updates_2d)
		delete mesh_2d;
}

void Batch::AddUpdate(Mesh *update_mesh)
{
	mesh_updates.insert(update_mesh);
}

void Batch::AddUpdate(Mesh2D *update_mesh_2d)
{
	mesh_updates_2d.insert(update_mesh_2d);
}

void Batch::CancelUpdate(Mesh *cancel_update_mesh)
{
	mesh_updates.erase(cancel_update_mesh);
}

void Batch::CancelUpdate(Mesh2D *cancel_update_mesh_2d)
{
	mesh_updates_2d.erase(cancel_update_mesh_2d);
}

void Batch::UpdateGPU()
{
	if (mesh_updates.empty() && mesh_updates_2d.empty()) return;

	// Calculate total size for both 3D and 2D meshes
	Uint32 total_size = 0;
	for (Mesh *mesh : mesh_updates)
	{
		total_size += mesh->size_vertices;
		total_size += mesh->size_indices;
	}
	for (Mesh2D *mesh : mesh_updates_2d)
	{
		total_size += mesh->size_vertices;
		total_size += mesh->size_indices;
	}

	// Create single transfer buffer for everything
	SDL_GPUTransferBufferCreateInfo transfer_info = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = total_size
	};
	SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(Drawing.Device(), &transfer_info);
	if (!transfer_buffer)
	{
		dbg_msg("&cBatch::UpdateGPU() Failed to create transfer buffer: %s\n", SDL_GetError());
		return;
	}

	// Map and copy all data
	void *mapped = SDL_MapGPUTransferBuffer(Drawing.Device(), transfer_buffer, false);
	Uint32 current_offset = 0;

	// Copy 3D mesh data
	for (Mesh *mesh : mesh_updates)
	{
		Uint32 vert_size = mesh->size_vertices;
		Uint32 idx_size = mesh->size_indices;

		// Store offsets for upload later
		mesh->transfer_vert_offset = current_offset;
		memcpy((char *)mapped + current_offset, mesh->vertices, vert_size);
		current_offset += vert_size;

		mesh->transfer_idx_offset = current_offset;
		memcpy((char *)mapped + current_offset, mesh->indices, idx_size);
		current_offset += idx_size;
	}

	// Copy 2D mesh data
	for (Mesh2D *mesh : mesh_updates_2d)
	{
		Uint32 vert_size = mesh->size_vertices;
		Uint32 idx_size = mesh->size_indices;

		// Store offsets for upload later
		mesh->transfer_vert_offset = current_offset;
		memcpy((char *)mapped + current_offset, mesh->vertices, vert_size);
		current_offset += vert_size;

		mesh->transfer_idx_offset = current_offset;
		memcpy((char *)mapped + current_offset, mesh->indices, idx_size);
		current_offset += idx_size;
	}

	SDL_UnmapGPUTransferBuffer(Drawing.Device(), transfer_buffer);

	// Start GPU commands
	SDL_GPUCommandBuffer *upload_cmd = SDL_AcquireGPUCommandBuffer(Drawing.Device());
	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(upload_cmd);

	// Upload all 3D meshes
	for (Mesh *mesh : mesh_updates)
	{
//		for (int i = 0; i < mesh->num_vertices; i++)
//		{
//			dbg_msg("Upload mesh vertex %i %i %i %i\n", mesh->vertices[i].color.r, mesh->vertices[i].color.g, mesh->vertices[i].color.b, mesh->vertices[i].color.a);
//		}

		Uint32 vert_size = mesh->size_vertices;
		Uint32 idx_size = mesh->size_indices;

		// Create GPU buffers if needed
		if (!mesh->gpu_vertices)
		{
			SDL_GPUBufferCreateInfo vert_info = {
				.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
				.size = vert_size
			};
			mesh->gpu_vertices = SDL_CreateGPUBuffer(Drawing.Device(), &vert_info);
		}
		if (!mesh->gpu_indices)
		{
			SDL_GPUBufferCreateInfo idx_info = {
				.usage = SDL_GPU_BUFFERUSAGE_INDEX,
				.size = idx_size
			};
			mesh->gpu_indices = SDL_CreateGPUBuffer(Drawing.Device(), &idx_info);
		}

		// Upload vertices
		SDL_GPUTransferBufferLocation vert_src = {
			.transfer_buffer = transfer_buffer,
			.offset = mesh->transfer_vert_offset
		};
		SDL_GPUBufferRegion vert_dst = {
			.buffer = mesh->gpu_vertices,
			.offset = 0,
			.size = vert_size
		};
		SDL_UploadToGPUBuffer(copy_pass, &vert_src, &vert_dst, false);

		// Upload indices
		SDL_GPUTransferBufferLocation idx_src = {
			.transfer_buffer = transfer_buffer,
			.offset = mesh->transfer_idx_offset
		};
		SDL_GPUBufferRegion idx_dst = {
			.buffer = mesh->gpu_indices,
			.offset = 0,
			.size = idx_size
		};
		SDL_UploadToGPUBuffer(copy_pass, &idx_src, &idx_dst, false);
	}

	// Upload all 2D meshes
	for (Mesh2D *mesh : mesh_updates_2d)
	{
		Uint32 vert_size = mesh->size_vertices;
		Uint32 idx_size = mesh->size_indices;

		// Create GPU buffers if needed
		if (!mesh->gpu_vertices)
		{
			SDL_GPUBufferCreateInfo vert_info = {
				.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
				.size = vert_size
			};
			mesh->gpu_vertices = SDL_CreateGPUBuffer(Drawing.Device(), &vert_info);
		}
		if (!mesh->gpu_indices)
		{
			SDL_GPUBufferCreateInfo idx_info = {
				.usage = SDL_GPU_BUFFERUSAGE_INDEX,
				.size = idx_size
			};
			mesh->gpu_indices = SDL_CreateGPUBuffer(Drawing.Device(), &idx_info);
		}

		// Upload vertices
		SDL_GPUTransferBufferLocation vert_src = {
			.transfer_buffer = transfer_buffer,
			.offset = mesh->transfer_vert_offset
		};
		SDL_GPUBufferRegion vert_dst = {
			.buffer = mesh->gpu_vertices,
			.offset = 0,
			.size = vert_size
		};
		SDL_UploadToGPUBuffer(copy_pass, &vert_src, &vert_dst, false);

		// Upload indices
		SDL_GPUTransferBufferLocation idx_src = {
			.transfer_buffer = transfer_buffer,
			.offset = mesh->transfer_idx_offset
		};
		SDL_GPUBufferRegion idx_dst = {
			.buffer = mesh->gpu_indices,
			.offset = 0,
			.size = idx_size
		};
		SDL_UploadToGPUBuffer(copy_pass, &idx_src, &idx_dst, false);
	}

	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(upload_cmd);
	SDL_WaitForGPUIdle(Drawing.Device());

	SDL_ReleaseGPUTransferBuffer(Drawing.Device(), transfer_buffer);
	mesh_updates.clear();
	mesh_updates_2d.clear();
}