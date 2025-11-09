//
// Created by Matq on 26/03/2023.
//

#include "Drawing.h"
#include <client/game/GameReference.h>
#include <client/core/Application.h>

SDL_GPUShader *LoadShader(SDL_GPUDevice *device, const char *filepath, SDL_GPUShaderCreateInfo shader_info)
{
	FILE *f = fopen(filepath, "rb");
	if (!f)
	{
		printf("ERROR: Could not open shader file: %s\n", filepath);
		return nullptr;
	}

	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);

//	printf("Loading %s: %zu bytes\n", filepath, size);  // ADD THIS

	unsigned char *code = new unsigned char[size];
	fread(code, 1, size, f);
	fclose(f);

	// Check SPIRV magic number
	if (size < 4 || *(uint32_t *)code != 0x07230203)
	{  // ADD THIS
		printf("ERROR: Invalid SPIRV magic number in %s\n", filepath);
		delete[] code;
		return nullptr;
	}

	shader_info.code_size = size;
	shader_info.code = code;
	SDL_GPUShader *shader = SDL_CreateGPUShader(device, &shader_info);

	dbg_msg("[Drawing] &9Loaded shader '%s'\n", filepath);

	delete[] code;
	return shader;
}

DrawingClass::DrawingClass()
{
	window = nullptr;
	gpu_device = nullptr;

	render_pass = nullptr;
	swapchain = nullptr;
	swapchain_w = 1;
	swapchain_h = 1;
	sampler = nullptr;

	depth_texture = nullptr;
	vertex_shader = nullptr;
	fragment_shader = nullptr;
	pipeline = nullptr;

	vertex_shader_2d = nullptr;
	fragment_shader_2d = nullptr;
	pipeline_2d = nullptr;

	status = Status::UNINITIALIZED;
}

void DrawingClass::Initialize(SDL_Window *drawing_window)
{
	window = drawing_window;

	Vec2i window_resolution = Application.GetResolution();

	{ // 3d stuff
		gpu_device = SDL_CreateGPUDevice(
			SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_DXIL,
			true,  // debug mode
			nullptr   // preferred device name
		);
		if (!gpu_device)
			throw std::runtime_error(Strings::FStringColors("Error while creating the gpu_device %s\n", SDL_GetError()));
		SDL_SetLogPriority(SDL_LOG_CATEGORY_GPU, SDL_LOG_PRIORITY_VERBOSE);
		SDL_SetLogPriority(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_VERBOSE);
		SDL_ClaimWindowForGPUDevice(gpu_device, window);

		// depth texture info
		SDL_GPUTextureCreateInfo depth_texture_info = {
			.type = SDL_GPU_TEXTURETYPE_2D,
			.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
			.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
			.width = static_cast<Uint32>(window_resolution.x),
			.height = static_cast<Uint32>(window_resolution.y),
			.layer_count_or_depth = 1,
			.num_levels = 1
		};
		depth_texture = SDL_CreateGPUTexture(gpu_device, &depth_texture_info);

		// shaders
		SDL_GPUShaderCreateInfo vertex_shader_info = {
			.entrypoint = "main",
			.format = SDL_GPU_SHADERFORMAT_SPIRV,
			.stage = SDL_GPU_SHADERSTAGE_VERTEX,
			.num_samplers = 0,
			.num_storage_textures = 0,
			.num_storage_buffers = 0,
			.num_uniform_buffers = 1,
		};
		vertex_shader = LoadShader(gpu_device, R"(.\vertex.spv)", vertex_shader_info);
		if (!vertex_shader)
		{
			SDL_Log("CRITICAL: Failed to load vertex shader: %s", SDL_GetError());
			throw std::runtime_error("Vertex shader loading failed");
		}

		SDL_GPUShaderCreateInfo fragment_shader_info = {
			.entrypoint = "main",
			.format = SDL_GPU_SHADERFORMAT_SPIRV,
			.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
			.num_samplers = 1,
			.num_storage_textures = 0,
			.num_storage_buffers = 0,
			.num_uniform_buffers = 1,
		};
		fragment_shader = LoadShader(gpu_device, R"(.\fragment.spv)", fragment_shader_info);
		if (!fragment_shader)
		{
			SDL_Log("CRITICAL: Failed to load fragment shader: %s", SDL_GetError());
			throw std::runtime_error("Fragment shader loading failed");
		}

		// vertex format
		SDL_GPUVertexBufferDescription vertex_binding_desc = {
			.slot = 0,
			.pitch = sizeof(GPUVertex),  // Your vertex struct size
			.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX
		};
		SDL_GPUVertexInputState vertex_input_state = {
			.vertex_buffer_descriptions = &vertex_binding_desc,
			.num_vertex_buffers = 1,
			.vertex_attributes = GPUVertex::vertex_attributes,
			.num_vertex_attributes =  sizeof(GPUVertex::vertex_attributes) / sizeof(GPUVertex::vertex_attributes[0])
		};

		// color target
//		SDL_GPUColorTargetDescription color_target_desc = { // todo: check if we need blending in 3d :p
//			.format = SDL_GetGPUSwapchainTextureFormat(gpu_device, window),
//			.blend_state = {
//				.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
//				.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
//				.color_blend_op = SDL_GPU_BLENDOP_ADD,
//				.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
//				.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO,
//				.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
//				.enable_blend = true,
//			}
//		};
		SDL_GPUColorTargetDescription color_target_desc = {
			.format = SDL_GetGPUSwapchainTextureFormat(gpu_device, window),
			.blend_state = {
				.enable_blend = false,
			}
		};

		// Create a sampler (one-time setup, not every frame)
		SDL_GPUSamplerCreateInfo sampler_info = {
			.min_filter = SDL_GPU_FILTER_LINEAR,
			.mag_filter = SDL_GPU_FILTER_LINEAR,
			.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
			.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
			.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		};
		sampler = SDL_CreateGPUSampler(gpu_device, &sampler_info);

		// pipeline
		SDL_GPUGraphicsPipelineCreateInfo pipeline_info = {
			.vertex_shader = vertex_shader,
			.fragment_shader = fragment_shader,
			.vertex_input_state = vertex_input_state,
			.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
			.rasterizer_state = {
				.fill_mode = SDL_GPU_FILLMODE_FILL,
				.cull_mode = SDL_GPU_CULLMODE_BACK,
				.front_face = SDL_GPU_FRONTFACE_CLOCKWISE
			},
			.depth_stencil_state = {
				.compare_op = SDL_GPU_COMPAREOP_LESS,
				.enable_depth_test = true,
				.enable_depth_write = true,
			},
			.target_info = {
				.color_target_descriptions = &color_target_desc,
				.num_color_targets = 1,
				.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
				.has_depth_stencil_target = true,
			},
		};
		pipeline = SDL_CreateGPUGraphicsPipeline(gpu_device, &pipeline_info);
		if (!pipeline)
		{
			SDL_Log("CRITICAL: Failed to create pipeline: %s", SDL_GetError());
			throw std::runtime_error("Pipeline creation failed");
		}
	}

	{
		// depth texture info
//		SDL_GPUTextureCreateInfo depth_texture_info = {
//			.type = SDL_GPU_TEXTURETYPE_2D,
//			.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM,
//			.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
//			.width = static_cast<Uint32>(window_resolution.x),
//			.height = static_cast<Uint32>(window_resolution.y),
//			.layer_count_or_depth = 1,
//			.num_levels = 1
//		};
//		depth_texture_2d = SDL_CreateGPUTexture(gpu_device, &depth_texture_info);

		// shaders
		SDL_GPUShaderCreateInfo vertex_shader_info = {
			.entrypoint = "main",
			.format = SDL_GPU_SHADERFORMAT_SPIRV,
			.stage = SDL_GPU_SHADERSTAGE_VERTEX,
			.num_samplers = 0,
			.num_storage_textures = 0,
			.num_storage_buffers = 0,
			.num_uniform_buffers = 1,
		};
		vertex_shader_2d = LoadShader(gpu_device, R"(.\vertex2d.spv)", vertex_shader_info);
		if (!vertex_shader_2d)
		{
			SDL_Log("CRITICAL: Failed to load vertex shader 2D: %s", SDL_GetError());
			throw std::runtime_error("Vertex shader loading failed");
		}

		SDL_GPUShaderCreateInfo fragment_shader_info = {
			.entrypoint = "main",
			.format = SDL_GPU_SHADERFORMAT_SPIRV,
			.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
			.num_samplers = 1,
			.num_storage_textures = 0,
			.num_storage_buffers = 0,
			.num_uniform_buffers = 1,
		};
		fragment_shader_2d = LoadShader(gpu_device, R"(.\fragment2d.spv)", fragment_shader_info);
		if (!fragment_shader_2d)
		{
			SDL_Log("CRITICAL: Failed to load fragment shader 2D: %s", SDL_GetError());
			throw std::runtime_error("Fragment shader loading failed");
		}

		// vertex format
		SDL_GPUVertexBufferDescription vertex_binding_desc_2d = {
			.slot = 0,
			.pitch = sizeof(GPUVertex2D),  // Your vertex struct size
			.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX
		};

		SDL_GPUVertexInputState vertex_input_state_2d = {
			.vertex_buffer_descriptions = &vertex_binding_desc_2d,
			.num_vertex_buffers = 1,
			.vertex_attributes = GPUVertex2D::vertex_attributes,
			.num_vertex_attributes = sizeof(GPUVertex2D::vertex_attributes) / sizeof(GPUVertex2D::vertex_attributes[0])
		};

		SDL_GPUColorTargetDescription color_target_2d = {
			.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM,
			.blend_state = {
				.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA,
				.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.color_blend_op = SDL_GPU_BLENDOP_ADD,
				.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE,
				.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
				.alpha_blend_op = SDL_GPU_BLENDOP_ADD,
				.enable_blend = true,
			}
		};

		// Create 2D pipeline
		SDL_GPUGraphicsPipelineCreateInfo pipeline_2d_info = {
			.vertex_shader = vertex_shader_2d,
			.fragment_shader = fragment_shader_2d,
			.vertex_input_state = vertex_input_state_2d,
			.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
			.rasterizer_state = {
				.fill_mode = SDL_GPU_FILLMODE_FILL,
				.cull_mode = SDL_GPU_CULLMODE_NONE,  // No culling for 2D
				.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
			},
			.depth_stencil_state = {
				.compare_op = SDL_GPU_COMPAREOP_LESS,   //
				.enable_depth_test = false,             //
				.enable_depth_write = false,            //
			},
			.target_info = {
				.color_target_descriptions = &color_target_2d,
				.num_color_targets = 1,
				.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM, //
				.has_depth_stencil_target = false,                        //
			},
		};
		pipeline_2d = SDL_CreateGPUGraphicsPipeline(gpu_device, &pipeline_2d_info);
		if (!pipeline_2d)
		{
			SDL_Log("CRITICAL: Failed to create pipeline_2d: %s", SDL_GetError());
			throw std::runtime_error("Pipeline creation failed");
		}
	}

	status = Status::INITIALIZED;
}

void DrawingClass::Destroy()
{
	if (status == Status::UNINITIALIZED)
		return;

	SDL_WaitForGPUIdle(gpu_device);
	shut_down_event.Invoke();

	// Release textures
	SDL_ReleaseGPUTexture(gpu_device, depth_texture);
//	auto it = cleanup_textures_shutdown.begin();
//	while (it != cleanup_textures_shutdown.end()) {
//		Texture *texture = *it;
//		it = cleanup_textures_shutdown.erase(it);
//		texture->Destroy();
//	}
//	cleanup_textures_shutdown.clear();
//	for (GPUMesh *gpu_mesh : gpu_meshes)
//	{
//		SDL_ReleaseGPUBuffer(gpu_device, gpu_mesh->vertex_buffer);
//		SDL_ReleaseGPUBuffer(gpu_device, gpu_mesh->indice_buffer);
//	}
//	gpu_meshes.clear();

	SDL_ReleaseGPUShader(gpu_device, vertex_shader);
	SDL_ReleaseGPUShader(gpu_device, fragment_shader);
	SDL_ReleaseGPUSampler(gpu_device, sampler);
	SDL_ReleaseGPUGraphicsPipeline(gpu_device, pipeline);

	// 2d
	SDL_ReleaseGPUShader(gpu_device, vertex_shader_2d);
	SDL_ReleaseGPUShader(gpu_device, fragment_shader_2d);
	SDL_ReleaseGPUGraphicsPipeline(gpu_device, pipeline_2d);

	if (window)
		SDL_ReleaseWindowFromGPUDevice(gpu_device, window);

	SDL_WaitForGPUIdle(gpu_device);
	SDL_DestroyGPUDevice(gpu_device);

	window = nullptr;
	gpu_device = nullptr;
	sampler = nullptr;

	depth_texture = nullptr;
	vertex_shader = nullptr;
	fragment_shader = nullptr;
	pipeline = nullptr;

	vertex_shader_2d = nullptr;
	fragment_shader_2d = nullptr;
	pipeline_2d = nullptr;

	status = Status::UNINITIALIZED;
}

DrawingClass::~DrawingClass()
{
	Destroy();
}

void DrawingClass::QueueUpdate(Mesh *update_mesh)
{
	mesh_updates.AddUpdate(update_mesh);
}

void DrawingClass::QueueUpdate(Mesh2D *update_mesh_2d)
{
	mesh_updates.AddUpdate(update_mesh_2d);
}

void DrawingClass::CancelUpdate(Mesh *cancel_update_mesh)
{
	mesh_updates.CancelUpdate(cancel_update_mesh);
}

void DrawingClass::CancelUpdate(Mesh2D *cancel_update_mesh_2d)
{
	mesh_updates.CancelUpdate(cancel_update_mesh_2d);
}

void DrawingClass::UpdateGPU()
{
	mesh_updates.UpdateGPU();
}

void DrawingClass::DrawFilledRect(const Dim2Rect& rect, const SDL_Color& sdl_color)
{
//	GPUVertex2D quad_vertices[Quad::NUM_VERTICES];
//	uint32_t quad_indices[Quad::NUM_INDICES];
//	Quad quad_rect(quad_vertices, quad_indices, rect, sdl_color);
//
//	Uint32 size_vertices = sizeof(quad_vertices);
//	Uint32 size_indices = sizeof(quad_indices);
//	Uint32 num_indices = size_indices; //
//
//	// 2. Create temporary buffers
//	SDL_GPUBufferCreateInfo vb_info = {
//		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
//		.size = size_vertices
//	};
//	SDL_GPUBuffer *vb = SDL_CreateGPUBuffer(gpu_device, &vb_info);
//
//	SDL_GPUBufferCreateInfo ib_info = {
//		.usage = SDL_GPU_BUFFERUSAGE_INDEX,
//		.size = size_indices
//	};
//	SDL_GPUBuffer *ib = SDL_CreateGPUBuffer(gpu_device, &ib_info);
//
//	// 3. Upload data
//	SDL_GPUTransferBufferCreateInfo transfer_info = {
//		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
//		.size = vb_info.size + ib_info.size
//	};
//	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(gpu_device, &transfer_info);
//
//	void *mapped = SDL_MapGPUTransferBuffer(gpu_device, transfer, false);
//	memcpy(mapped, quad_vertices, vb_info.size);
//	memcpy((char *)mapped + vb_info.size, quad_indices, ib_info.size);
//	SDL_UnmapGPUTransferBuffer(gpu_device, transfer);
//
//	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gpu_device);
//	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);
//
//	SDL_GPUTransferBufferLocation vert_src = { .transfer_buffer = transfer, .offset = 0 };
//	SDL_GPUBufferRegion vert_dst = { .buffer = vb, .offset = 0, .size = vb_info.size };
//	SDL_UploadToGPUBuffer(copy_pass, &vert_src, &vert_dst, false);
//
//	SDL_GPUTransferBufferLocation idx_src = { .transfer_buffer = transfer, .offset = vb_info.size };
//	SDL_GPUBufferRegion idx_dst = { .buffer = ib, .offset = 0, .size = ib_info.size };
//	SDL_UploadToGPUBuffer(copy_pass, &idx_src, &idx_dst, false);
//
//	SDL_EndGPUCopyPass(copy_pass);
//	SDL_SubmitGPUCommandBuffer(cmd);
//	SDL_ReleaseGPUTransferBuffer(gpu_device, transfer);

	// 4. Store for rendering
//	draw_commands.push_back(
//		{
//			.vertex_buffer = vb,
//			.index_buffer = ib,
//			.index_count = 6
//		}
//	);
}

bool DrawingClass::CreateCommandBuffer()
{
	cmd = SDL_AcquireGPUCommandBuffer(gpu_device);
	if (!cmd)
	{
		SDL_Log("ERROR: Failed to acquire command buffer: %s", SDL_GetError());
		return false;
	}

	SDL_AcquireGPUSwapchainTexture(cmd, window, &swapchain, &swapchain_w, &swapchain_h);

	bool success = swapchain != nullptr;
	if (!success)
		return false;

	return true;
}

bool DrawingClass::BeginPass()
{
	SDL_GPUColorTargetInfo color_target = {
		.texture = swapchain,
		.clear_color = (SDL_FColor){ 0, 0, 0, 1.0f },
		.load_op = SDL_GPU_LOADOP_CLEAR,
//			.clear_color = (SDL_FColor){ 1.0f, 0.0f, 1.0f, 1.0f },
//			.load_op = SDL_GPU_LOADOP_LOAD,
		.store_op = SDL_GPU_STOREOP_STORE,
	};

	SDL_GPUDepthStencilTargetInfo depth_target = {
		.texture = depth_texture,
		.clear_depth = 1.0f,
		.load_op = SDL_GPU_LOADOP_CLEAR,
		.store_op = SDL_GPU_STOREOP_DONT_CARE,
	};

	render_pass = SDL_BeginGPURenderPass(
		cmd,
		&color_target,
		1,
		&depth_target
	);

	if (!render_pass)
	{
		SDL_Log("ERROR: Failed to begin render pass: %s", SDL_GetError());
		return false;
	}
	return true;
}

void DrawingClass::SetPipeline3D()
{
	SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
}

void DrawingClass::ShaderTick()
{
	Camera3D& camera = GameReference.GetCamera3D();

	// vertex
	memcpy(vertex_uniform.view, camera.GetViewMatrix().m, sizeof(float) * 16);
	memcpy(vertex_uniform.projection, camera.GetProjectionMatrix().m, sizeof(float) * 16);

	// fragment
	fragment_uniform.light_dir = Vec3f(-0.5f, -0.5f, -0.5f).NormalizeF();
}

void DrawingClass::SetViewportAndScissor()
{
	SDL_GPUViewport viewport = {
		.x = 0,
		.y = 0,
		.w = (float)swapchain_w,
		.h = (float)swapchain_h,
		.min_depth = 0.0f,
		.max_depth = 1.0f
	};
	SDL_SetGPUViewport(render_pass, &viewport);

	SDL_Rect scissor = {
		.x = 0,
		.y = 0,
		.w = (int)swapchain_w,
		.h = (int)swapchain_h
	};
	SDL_SetGPUScissor(render_pass, &scissor);
}

void DrawingClass::ShaderParams(const Mat4x4& model_matrix, bool use_texture)
{
	memcpy(vertex_uniform.model, model_matrix.m, sizeof(float) * 16);
	SDL_PushGPUVertexUniformData(cmd, 0, &vertex_uniform, sizeof(VertexUniform));

	fragment_uniform.use_texture = use_texture ? 1.0f : 0.0f;
	SDL_PushGPUFragmentUniformData(cmd, 0, &fragment_uniform, sizeof(FragmentUniform));
}

bool DrawingClass::BeginPass2D()
{
	// 2D color target info (no depth)
	SDL_GPUColorTargetInfo color_target = {
		.texture = swapchain,
		.clear_color = (SDL_FColor){ 0.0f, 0.0f, 0.0f, 0.0f }, // optional: transparent or keep previous
		.load_op = SDL_GPU_LOADOP_LOAD,   // load existing contents (3D scene)
		.store_op = SDL_GPU_STOREOP_STORE,
	};

//	SDL_GPUDepthStencilTargetInfo depth_target = {
//		.texture = depth_texture_2d,
//		.clear_depth = 1.0f, // Clear to far plane
//		.load_op = SDL_GPU_LOADOP_CLEAR,
//		.store_op = SDL_GPU_STOREOP_DONT_CARE,
//	};

	// Begin render pass without depth/stencil
	render_pass = SDL_BeginGPURenderPass(
		cmd,
		&color_target,
		1,
		nullptr   // no depth/stencil target
	);

	if (!render_pass)
	{
		SDL_Log("ERROR: Failed to begin 2D render pass: %s", SDL_GetError());
		SDL_SubmitGPUCommandBuffer(cmd);
		return false;
	}
	return true;
}

void DrawingClass::SetPipeline2D()
{
	SDL_BindGPUGraphicsPipeline(render_pass, pipeline_2d);
}

void DrawingClass::ShaderTick2D()
{
	vertex_uniform_2d.screen_size = Vec2f(Application.GetResolution());
}

void DrawingClass::ShaderParams2D(const Vec2f& translation, float rotation, bool use_texture, float draw_as_circles)
{
	vertex_uniform_2d.translation = translation;
	vertex_uniform_2d.rotation = rotation;
	SDL_PushGPUVertexUniformData(cmd, 0, &vertex_uniform_2d, sizeof(VertexUniform2D));

	fragment_uniform_2d.use_texture = use_texture ? 1.0f : 0.0f;
	fragment_uniform_2d.draw_as_circle = draw_as_circles ? 1.0f : 0.0f;
	SDL_PushGPUFragmentUniformData(cmd, 0, &fragment_uniform_2d, sizeof(FragmentUniform2D));
}

void DrawingClass::EndPass()
{
	SDL_EndGPURenderPass(render_pass);
}

void DrawingClass::EndCommandBuffer()
{
	SDL_SubmitGPUCommandBuffer(cmd);
}

//GPUMesh *DrawingClass::LoadMeshToGPU(const Mesh& mesh)
//{
//	auto& vertices = mesh.vertices;
//	auto& indices = mesh.indices;
//	auto& normals = mesh.face_normals;
//	auto& uvs = mesh.uvs;
//
//	std::vector<GPUVertex> gpu_vertices;
//	gpu_vertices.reserve(vertices.size());
//
//	for (size_t i = 0; i < vertices.size(); i++)
//		gpu_vertices.push_back(
//			{
//				.x = vertices[i].x,
//				.y = vertices[i].y,
//				.z = vertices[i].z,
//				.nx = normals[i].x,
//				.ny = normals[i].y,
//				.nz = normals[i].z,
//				.u = uvs[i].x,
//				.v = uvs[i].y,
//				.color = {
//					.r = 255,
//					.g = 255,
//					.b = 255,
//					.a = 255
//				}
//			}
//		);
//
//	// Convert indices to uint32_t
//	std::vector<uint32_t> gpu_indices(indices.begin(), indices.end());
//
//	// 2. Create vertex buffer
//	SDL_GPUBufferCreateInfo vertex_buffer_info = {
//		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
//		.size = (Uint32)(gpu_vertices.size() * sizeof(GPUVertex))
//	};
//	SDL_GPUBuffer *vertex_buffer = SDL_CreateGPUBuffer(gpu_device, &vertex_buffer_info);
//
//	// 3. Create index buffer
//	SDL_GPUBufferCreateInfo index_buffer_info = {
//		.usage = SDL_GPU_BUFFERUSAGE_INDEX,
//		.size = (Uint32)(gpu_indices.size() * sizeof(uint32_t))
//	};
//	SDL_GPUBuffer *index_buffer = SDL_CreateGPUBuffer(gpu_device, &index_buffer_info);
//
//	// 4. Create transfer buffer (big enough for both)
//	Uint32 total_size = vertex_buffer_info.size + index_buffer_info.size;
//	SDL_GPUTransferBufferCreateInfo transfer_info = {
//		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
//		.size = total_size
//	};
//	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(gpu_device, &transfer_info);
//
//	// 5. Map and copy data
//	void *mapped = SDL_MapGPUTransferBuffer(gpu_device, transfer, false);
//	memcpy(mapped, gpu_vertices.data(), vertex_buffer_info.size);
//	memcpy((char *)mapped + vertex_buffer_info.size, gpu_indices.data(), index_buffer_info.size);
//	SDL_UnmapGPUTransferBuffer(gpu_device, transfer);
//
//	// 6. Upload to GPU
//	SDL_GPUCommandBuffer *upload_cmd = SDL_AcquireGPUCommandBuffer(gpu_device);
//	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(upload_cmd);
//
//	// Upload vertices
//	SDL_GPUTransferBufferLocation vert_src = {
//		.transfer_buffer = transfer,
//		.offset = 0
//	};
//	SDL_GPUBufferRegion vert_dst = {
//		.buffer = vertex_buffer,
//		.offset = 0,
//		.size = vertex_buffer_info.size
//	};
//	SDL_UploadToGPUBuffer(copy_pass, &vert_src, &vert_dst, false);
//
//	// Upload indices
//	SDL_GPUTransferBufferLocation idx_src = {
//		.transfer_buffer = transfer,
//		.offset = vertex_buffer_info.size
//	};
//	SDL_GPUBufferRegion idx_dst = {
//		.buffer = index_buffer,
//		.offset = 0,
//		.size = index_buffer_info.size
//	};
//	SDL_UploadToGPUBuffer(copy_pass, &idx_src, &idx_dst, false);
//
//	SDL_EndGPUCopyPass(copy_pass);
//	SDL_SubmitGPUCommandBuffer(upload_cmd);
//
//	// 7. Cleanup transfer buffer
//	SDL_ReleaseGPUTransferBuffer(gpu_device, transfer);
//
//	auto *new_gpu_mesh = new GPUMesh();
//	new_gpu_mesh->vertex_buffer = vertex_buffer;
//	new_gpu_mesh->indice_buffer = index_buffer;
//	new_gpu_mesh->indice_count = (uint32_t)gpu_indices.size();
//	new_gpu_mesh->gpu_texture = mesh.gpu_texture;
//	new_gpu_mesh->position = mesh.position;
//
//	gpu_meshes.push_back(new_gpu_mesh);
//	return new_gpu_mesh;
//}

void DrawingClass::DrawTriangles(SDL_GPUBuffer *vertices, SDL_GPUBuffer *indices, Uint32 num_indices, SDL_GPUTexture *texture)
{
	SDL_GPUBufferBinding vb_bindings = { .buffer = vertices, .offset = 0 };
	SDL_BindGPUVertexBuffers(render_pass, 0, &vb_bindings, 1);
	SDL_GPUBufferBinding ib_binding = { .buffer = indices, .offset = 0 };
	SDL_BindGPUIndexBuffer(render_pass, &ib_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

	texture = texture != nullptr ? texture : Assets.GetInvalidTexture()->GPUTexture();
	SDL_GPUTextureSamplerBinding texture_binding = { .texture = texture, .sampler = sampler };
	SDL_BindGPUFragmentSamplers(render_pass, 0, &texture_binding, 1);

	SDL_DrawGPUIndexedPrimitives(render_pass, num_indices, 1, 0, 0, 0);
}

DrawingClass Drawing;
