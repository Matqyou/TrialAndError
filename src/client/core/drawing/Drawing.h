//
// Created by Matq on 26/03/2023.
//

#pragma once

#include <client/core/drawing/Camera3D.h>
#include <client/core/drawing/Camera.h>
#include <client/core/drawing/Mesh.h>
#include <client/core/Assets.h>
#include <shared/screen/Dim2Rect.h>
#include <SDL3_image/SDL_image.h>

struct GPUVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	SDL_Color color;

	static SDL_GPUVertexAttribute vertex_attributes[];
};

struct GPUVertex2D
{
	float x, y;
	float sx, sy;
	float u, v;
	SDL_Color color;

	static SDL_GPUVertexAttribute vertex_attributes[];
};

class Quad;
class DrawCall
{
private:
	friend class Quad;
	GPUVertex2D *vertices;
	Uint32 num_vertices;
	uint32_t *indices;
	Uint32 num_indices;

	SDL_GPUBuffer *gpu_vertices;
	Uint32 size_vertices;
	SDL_GPUBuffer *gpu_indices;
	Uint32 size_indices;

	// vert
	Vec2f translation;
	float rotation;

	// frag
	SDL_GPUTexture *texture;
	bool draw_as_circles;

	Uint32 current_vertices_offset;
	Uint32 current_indices_offset;

public:
	DrawCall(Uint32 init_num_vertices, Uint32 init_num_indices, SDL_GPUTexture *init_texture = nullptr);
	~DrawCall();

	GPUVertex2D *Vertices() { return vertices; }
	uint32_t *Indices() { return indices; }
//	Uint32 GetNumIndices() { return num_indices; }
//	SDL_GPUTexture *Texture() { return texture; }
//	float Rotation() { return rotation; }

//	SDL_GPUBuffer *GPUVertices() { return gpu_vertices; }
//	Uint32 SizeGPUVertices() { return size_vertices; }
//	SDL_GPUBuffer *GPUIndices() { return gpu_indices; }
//	Uint32 SizeGPUIndices() { return size_indices; }

	void SetTexture(SDL_GPUTexture *new_texture) { texture = new_texture; } // Do not require UpdateGPU()
	void SetDrawAsCircles(bool new_draw_as_circles) { draw_as_circles = new_draw_as_circles; }

	void SetTranslation(const Vec2f& new_translation) { translation = new_translation; } // Do not require UpdateGPU()
	void SetRotation(float new_rotation) { rotation = new_rotation; } // Do not require UpdateGPU()
//	void SetZ(float new_z) { z = new_z; }

	void UpdateGPU(); // Only updates vertices and indices(and count)
	void Draw();

};

class Quad
{
public:
	static const Uint32 NUM_VERTICES;
	static const Uint32 NUM_INDICES;

private:
	GPUVertex2D *vertices;
	uint32_t *indices;

public:
	Quad();
	Quad(GPUVertex2D *vertices_address, uint32_t *indices_address, const Dim2Rect& start_rect, const SDL_Color& start_color = { 255, 255, 255, 255 });
	Quad(DrawCall& draw_command, const Dim2Rect& start_rect, const SDL_Color& start_color = { 255, 255, 255, 255 });

	GPUVertex2D& TopLeft() { return vertices[0]; }
	GPUVertex2D& TopRight() { return vertices[1]; }
	GPUVertex2D& BottomLeft() { return vertices[2]; }
	GPUVertex2D& BottomRight() { return vertices[3]; }

	void Bind(GPUVertex2D *vertices_address, uint32_t *indices_address, const Dim2Rect& start_rect, const SDL_Color& start_color = { 255, 255, 255, 255 });
	void Bind(DrawCall& draw_command, const Dim2Rect& start_rect, const SDL_Color& start_color = { 255, 255, 255, 255 });
	void UpdateRect(const Dim2Rect& new_rect);
	void UpdateColor(const SDL_Color& new_color);
};

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
	SDL_GPUTexture *depth_texture_2d;

	// 3d stuff
	SDL_GPUShader *vertex_shader;
	SDL_GPUShader *fragment_shader;
	SDL_GPUGraphicsPipeline *pipeline;
	VertexUniform vertex_uniform;
	FragmentUniform fragment_uniform;

	std::vector<SDL_GPUTexture *> gpu_textures; // move to assets?
	std::vector<GPUMesh *> gpu_meshes; // move to assets?

	// 2d stuff
	SDL_GPUShader *vertex_shader_2d;
	SDL_GPUShader *fragment_shader_2d;
	SDL_GPUGraphicsPipeline *pipeline_2d;
	VertexUniform2D vertex_uniform_2d;
	FragmentUniform2D fragment_uniform_2d;

	Status status;

public:
	DrawingClass();
	~DrawingClass();

	void Initialize(SDL_Window *drawing_window);
	void Destroy();

	// Getting
	[[nodiscard]] SDL_GPUDevice *Device() const { return gpu_device; }
	[[nodiscard]] SDL_GPUCommandBuffer *CommandBuffer() const { return cmd; }
//	[[nodiscard]] SDL_Renderer *Renderer() const { return renderer; }

	// Options
//	void SetRenderTarget(Texture *target);
//	void SetRenderTargetSDL(SDL_Texture *target);
//	void SetDrawBlendMode(SDL_BlendMode blend_mode) { SDL_SetRenderDrawBlendMode(renderer, blend_mode); }
//	void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { SDL_SetRenderDrawColor(renderer, r, g, b, a); }
//	void SetColor(SDL_Color color) { SetColor(color.r, color.g, color.b, color.a); }

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
	SDL_GPUTexture *LoadTextureToGPU(SDL_Texture *sdl_texture);
	GPUMesh *LoadMeshToGPU(const Mesh& mesh);
//	void UpdateRender();

	// 2d
	void DrawTriangles(
		SDL_GPUBuffer *vertices,
		SDL_GPUBuffer *indices,
		Uint32 num_indices,
		SDL_GPUTexture *texture
	);

	// pass

};

extern DrawingClass Drawing;
