//
// Created by Matq on 26/03/2023.
//

#pragma once

#include <client/core/drawing/mesh/Mesh2D.h>
#include <client/core/drawing/mesh/Batch.h>
#include <client/core/drawing/mesh/Mesh.h>
#include <client/core/drawing/Camera3D.h>
#include <client/core/drawing/Camera.h>

#include <client/core/Assets.h>
#include <shared/code/EventChannel.h>
#include <shared/screen/Dim2Rect.h>

#include <shared/code/EventOnce.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_set>

struct VertexUniform
{
	float model[16];
	float view[16];
	float projection[16];
};

struct FragmentUniform
{
	Vec3f light_dir;
	float use_texture;
};

struct VertexUniform2D
{
	Vec2f screen_size;
	Vec2f translation;
	float rotation;
//	float z;
};

struct FragmentUniform2D
{
	float use_texture;
	float draw_as_circle;
};

class GameData;
class GameWorld;
class DrawingClass
{
public:
	enum class Status
	{
		UNINITIALIZED,
		INITIALIZED,
	};

private:
	SDL_Window *window;
	SDL_GPUDevice *gpu_device;

	// rendering
	SDL_GPURenderPass *render_pass;
	SDL_GPUTexture *swapchain;
	Uint32 swapchain_w, swapchain_h;
	SDL_GPUCommandBuffer *cmd;
	SDL_GPUSampler *sampler;
	SDL_GPUTexture *depth_texture;
//	SDL_GPUTexture *depth_texture_2d;

	// 3d stuff
	SDL_GPUShader *vertex_shader;
	SDL_GPUShader *fragment_shader;
	SDL_GPUGraphicsPipeline *pipeline;
	VertexUniform vertex_uniform;
	FragmentUniform fragment_uniform;

//	std::vector<GPUMesh *> gpu_meshes; // move to assets?
//	std::unordered_set<GPUMesh*> cleanup_meshes;

	// 2d stuff
	SDL_GPUShader *vertex_shader_2d;
	SDL_GPUShader *fragment_shader_2d;
	SDL_GPUGraphicsPipeline *pipeline_2d;
	VertexUniform2D vertex_uniform_2d;
	FragmentUniform2D fragment_uniform_2d;

	//
	EventChannel<> shut_down_event;
	Batch mesh_updates;

	Status status;

public:
	DrawingClass();
	void Initialize(SDL_Window *drawing_window);
	void Destroy();
	~DrawingClass();

	// Getting
	[[nodiscard]] SDL_GPUDevice *Device() const { return gpu_device; }
	[[nodiscard]] SDL_GPUCommandBuffer *CommandBuffer() const { return cmd; }

	// Events
	[[nodiscard]] EventChannel<>& ShutDownEvent() { return shut_down_event; }

	// Options
//	void SetRenderTarget(Texture *target);
//	void SetRenderTargetSDL(SDL_Texture *target);
//	void SetDrawBlendMode(SDL_BlendMode blend_mode) { SDL_SetRenderDrawBlendMode(renderer, blend_mode); }
//	void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { SDL_SetRenderDrawColor(renderer, r, g, b, a); }
//	void SetColor(SDL_Color color) { SetColor(color.r, color.g, color.b, color.a); }

	// Manipulating
	void QueueUpdate(Mesh *update_mesh);
	void QueueUpdate(Mesh2D *update_mesh_2d);
	void CancelUpdate(Mesh *cancel_update_mesh);
	void CancelUpdate(Mesh2D *cancel_update_mesh_2d);
	void UpdateGPU();

	// Drawing
	void DrawFilledRect(const Dim2Rect& rect, const SDL_Color& sdl_color);
//	void DrawRect(const SDL_FRect& rect, bool fill);
//	void DrawRect(const SDL_FRect& rect, bool fill, const Camera& camera);
//	void FillCircle(const Vec2f& center, float radius, SDL_FColor color);
//	void DrawLine(const Vec2f& start, const Vec2f& end);
//	void DrawLine(const Vec2f& start, const Vec2f& end, const Camera& camera);
//	void DrawLine(const Vec2f& start, const Vec2f& end, float size, SDL_FColor color);
//	void DrawLine(const Vec2f& start, const Vec2f& end, float size, SDL_FColor color, const Camera& camera);
//	void RenderTexture(SDL_Texture *texture, SDL_FRect *srcrect, const SDL_FRect& dstrect);
//	void RenderTexture(SDL_Texture *texture, SDL_FRect *srcrect, const SDL_FRect& dstrect, const Camera& camera);
//	void RenderTextureRotated(SDL_Texture *texture,
//							  SDL_FRect *srcrect, const SDL_FRect& dstrect,
//							  double angle, SDL_FPoint *center,
//							  SDL_FlipMode flip);
//	void RenderTextureRotated(SDL_Texture *texture,
//							  SDL_FRect *srcrect, const SDL_FRect& dstrect,
//							  double angle, SDL_FPoint *center,
//							  SDL_FlipMode flip, const Camera& camera);
//	void RenderTextureFullscreen(SDL_Texture *texture, SDL_FRect *srcrect);
//	void Clear();  // Resets all pixels to a color value
//	void FillAll();  // If color blending is enabled, fill the window with a semi-transparent tone
//	void UpdateWindow();  // Pushes the latest state of the pixels to window

	bool CreateCommandBuffer();
	bool BeginPass();
	void SetPipeline3D();
	void ShaderTick();
	void SetViewportAndScissor();
	void ShaderParams(const Mat4x4& model_matrix, bool use_texture);

	bool BeginPass2D();
	void SetPipeline2D();
	void ShaderTick2D();
	void ShaderParams2D(const Vec2f& translation, float rotation, bool use_texture, float draw_as_circles);

	void EndPass();
	void EndCommandBuffer();

	// 3d stuff
//	SDL_GPUTexture *LoadTextureToGPU(SDL_Texture *sdl_texture);
//	GPUMesh *LoadMeshToGPU(const Mesh& mesh);


	void DrawTriangles(
		SDL_GPUBuffer *vertices,
		SDL_GPUBuffer *indices,
		Uint32 num_indices,
		SDL_GPUTexture *texture
	);

};

extern DrawingClass Drawing;
