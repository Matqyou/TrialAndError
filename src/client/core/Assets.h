//
// Created by Mat on 12/3/2024.
//

#pragma once

#include <client/core/texture/visual/VisualSurface.h>
#include <client/core/texture/visual/VisualTexture.h>

#include <shared/string/Strings.h>
#include <shared/math/Vec2.h>

#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>

class Sound
{
private:
	friend class AssetsClass;
	const std::string m_Key;
	MIX_Audio *mix_audio;
	std::string m_LoadExtension;

public:
	explicit Sound(std::string key = "NaN", MIX_Audio *init_mix_audio = nullptr, std::string load_extension = "NaN");
	~Sound();

	// Getting
	[[nodiscard]] MIX_Audio *MixAudio() const { return mix_audio; }

	// Manipulating
	void SetVolume(int volume);
	void PlaySound();
};

class Font
{
private:
	friend class AssetsClass;
	const std::string m_Key;
	std::string m_LoadExtension;
	TTF_Font *m_TTFFont;
	float m_Size;

public:
	explicit Font(TTF_Font *ttf_font = nullptr, std::string key = "NaN", std::string load_extension = "NaN");
	~Font();

	// Getting
	[[nodiscard]] TTF_Font *TTFFont() const { return m_TTFFont; }

	// Manipulating
	Font *CacheSize(float size)
	{
		this->m_Size = size;
		return this;
	}

};

class DrawingClass;
class LoadTexture;
class PregenerateTexture;
class LinkSound;
class PreloadFont;
class LinkFont;
class AssetsClass
{
public:
	enum class Status
	{
		// Nothing has been initialized as of this point.
		UNINITIALIZED,

		// Assets such as - images, sounds, fonts - now have
		// their keys loaded into the memory, and you can access
		// them using LinkTexture, LinkSound, LinkMusic and LinkFont.
		KEYS_LOADED,

		// All the names used throughout the program such as
		// LinkTexture, LinkSound, LinkMusic and LinkFont should
		// now be populated with values.
		ASSETS_LINKED,
	};

	enum class TexturePurpose
	{
		// UI, HUD, sprites
		GUI_ELEMENT,

		// Characters, props, world geometry
		OBJECT_3D,

		// Framebuffer for post-processing
		RENDER_TARGET,

		// Depth/stencil attachment
		DEPTH_BUFFER,

		// Skybox, reflections
		CUBEMAP,

		// Sprite sheets, tile maps
		TEXTURE_ATLAS,
	};

private:
	bool m_SoundsEnabled;

	using Resources = std::vector<std::tuple<std::string, std::string, std::string>>;
	Resources m_VisHitboxResources;
	Resources m_SurfaceResources;
	Resources m_SoundResources;
	Resources m_FontResources;
	size_t m_SurfaceResourcesIndex;
	size_t m_SoundResourcesIndex;

	std::unordered_set<Surface*> cleanup_surfaces;
	std::unordered_set<Texture*> cleanup_textures;

	// Textures
	std::unordered_map<std::string, Surface *> m_Surfaces;
	std::unordered_map<std::string, Texture *> m_Textures;
	static std::vector<LoadTexture *> m_LoadTextures;
	static std::vector<PreloadFont *> m_PreloadFonts;
	static std::vector<LinkSound *> m_LinkSounds;
	static std::vector<LinkFont *> m_LinkFonts;
	Surface *m_InvalidSurfaceDefault;
	Texture *m_InvalidTextureDefault;

	std::unordered_map<std::string, Sound *> m_Sounds;
	std::unordered_map<std::string, Font *> m_Fonts;

	// Iterators
	std::vector<PreloadFont *>::iterator m_PreloadFontIterator;

	static size_t sTotalWork, sWorkDone;
	std::vector<std::string> failed_load_surfaces;
	std::vector<std::string> failed_load_textures;
	std::vector<std::string> failed_load_sounds;
	std::vector<std::string> failed_load_fonts;
	std::vector<std::string> failed_link_sounds;
	std::vector<std::string> failed_link_fonts;

	size_t success_load_surfaces;
	size_t success_load_textures;
	size_t success_load_sounds;
	size_t success_load_fonts;
	size_t success_link_sounds;
	size_t success_link_fonts;

	bool LoadingSurfaces();
	bool LoadingSounds();
	bool LoadingFonts(); // All at once
//	bool GeneratingTextures();
	bool LoadingTextures(); // All at once
	bool LinkingSounds(); // All at once
	bool LinkingFonts(); // All at once

	Status status; // To know if assets are initialized or not

	SDL_GPUTextureCreateInfo GetGPUTextureCreateInfo(const Vec2i& texture_size, TexturePurpose purpose);
	SDL_GPUTexture *GPUTextureFromSurface(SDL_Surface *sdl_surface, TexturePurpose purpose);

public:
	AssetsClass();
	void Initialize(bool sounds_enabled);
	void Destroy();
	~AssetsClass();

	// Getting
	[[nodiscard]] Status GetStatus() const { return status; }
	[[nodiscard]] Texture *GetInvalidTexture() const { return m_InvalidTextureDefault; };
	[[nodiscard]] Surface *GetSurface(const std::string& surface_key);
	[[nodiscard]] Texture *GetTexture(const std::string& texture_key);
	[[nodiscard]] const std::unordered_map<std::string, Texture *>& GetAllTextures();
	[[nodiscard]] Sound *GetSound(const std::string& sound_key);
	[[nodiscard]] Font *GetFont(const std::string& font_key);
	[[nodiscard]] bool SoundsEnabled() const { return m_SoundsEnabled; }
	[[nodiscard]] bool IsLoading() { return status != Status::ASSETS_LINKED; }
	[[nodiscard]] static size_t GetTotalWork() { return sTotalWork; }
	[[nodiscard]] static size_t GetWorkDone() { return sWorkDone; }

	// Generating
	Texture *TextureFromSurface(const Surface& surface, TexturePurpose purpose);
	SDL_Surface *CreateSDLSurface(int width, int height, SDL_PixelFormat format);
	Texture *CreateTexture(TexturePurpose purpose, const Vec2i& size, Rect4f *visual_hitbox = nullptr);
//    VisualTexture* RenderTextBlendedVisual(TTF_Font* font, const std::string& text, SDL_Color color);
	Texture *RenderTextBlended(TTF_Font *font, const std::string& text, SDL_Color color);
	Texture *RenderTextBlended(TTF_Font *font, const char *text, SDL_Color color);
//	Texture *RenderTextBlendedOutline(TTF_Font *font, const char *text, int thickness,
//									  SDL_Color text_color, SDL_Color outline_color);
	Texture *RenderTextSolid(TTF_Font *font, const std::string& text, SDL_Color color);
	Texture *RenderTextSolid(TTF_Font *font, const char *text, SDL_Color color);
//	Texture *RenderTextureOnTextureCentered(Texture *outer, Texture *inner, bool copy = false);
//	bool SaveTextureToDisk(Texture *texture, const std::string& filename);

	// Manipulating
	void NewSurface(Surface *new_surface);
	void RemoveSurface(Surface *remove_surface);
	void NewTexture(Texture *new_texture);
	void RemoveTexture(Texture *remove_texture);
	static void AddLoadTexture(LoadTexture *load_texture);
//	static void LinkPregeneratedTexture(PregenerateTexture *pregenerate_texture);
	static void LinkPreloadedSound(LinkSound *register_sound);
	static void PreloadFont_(PreloadFont *preload_font);
	static void LinkPreloadedFont(LinkFont *register_font);
	void ThreadedLoading();

};

extern AssetsClass Assets;

// when creating the LinkTexture object, it saves the key you provide
//  and returns an object which can be retrieved with either *object
//  or by using object.GetTexture() / object.GetSDLTexture()
// the value is only assigned at startup
class LoadTexture
{
private:
	friend class AssetsClass;
	std::string key;
	std::string from_surface_key;
	AssetsClass::TexturePurpose texture_purpose;
	Texture *texture;

//	using TextureCallback = std::function<void(Texture *)>;
//	TextureCallback m_LoadCallback;

public:
	explicit LoadTexture(std::string new_key, AssetsClass::TexturePurpose purpose);
	explicit LoadTexture(std::string new_key, std::string surface_key, AssetsClass::TexturePurpose purpose);
//	explicit LoadTexture(std::string texture_key, TextureCallback load_callback);
	operator Texture *() const { return GetTexture(); }  // Implicit
	operator SDL_GPUTexture *() const { return GetGPUTexture(); }  // Implicit

	// Getting
	[[nodiscard]] const std::string& Key() const { return key; }
	[[nodiscard]] const std::string& FromSurfaceKey() const { return from_surface_key; }
	[[nodiscard]] AssetsClass::TexturePurpose GetPurpose() const { return texture_purpose; }
	[[nodiscard]] Texture *GetTexture() const { return texture; } // todo: error handling
	[[nodiscard]] SDL_GPUTexture *GetGPUTexture() const { return texture->GPUTexture(); } // todo: error handling

};

//class PregenerateTexture
//{
//private:
//	friend class AssetsClass;
//	std::string m_Key;
//	Texture *m_Texture;
//
//	using TextureCallback = std::function<Texture *(AssetsClass *)>;
//	TextureCallback m_GenerateCallback;
//
//public:
//	explicit PregenerateTexture(std::string texture_key, TextureCallback generate_callback);
//
//	// Getting
//	[[nodiscard]] const std::string& Key() const { return m_Key; }
//	[[nodiscard]] Texture *GetTexture() const { return m_Texture; } // todo: error handling
//	[[nodiscard]] SDL_GPUTexture *GetGPUTexture() const
//	{
//		return m_Texture->GPUTexture(); // todo: error handling
//	}
//
//};

class LinkSound
{
private:
	friend class AssetsClass;
	std::string m_Key;
	Sound *m_Sound;

public:
	explicit LinkSound(std::string sound_key);

	// Getting
	[[nodiscard]] const std::string& Key() const { return m_Key; }
	[[nodiscard]] Sound *GetSound() const;

};

class PreloadFont
{
private:
	friend class AssetsClass;
	std::string m_Key;
	std::string m_FontID;
	float m_Size;
	Font *m_Font;

public:
	explicit PreloadFont(std::string font_key, std::string font_id, float ptsize);
	operator Font *() const { return GetFont(); }  // Implicit
	operator TTF_Font *() const { return GetTTFFont(); }  // Implicit

	// Getting
	[[nodiscard]] const std::string& Key() const { return m_Key; }
	[[nodiscard]] const std::string& FontID() const { return m_FontID; }
	[[nodiscard]] float Size() const { return m_Size; }
	[[nodiscard]] Font *GetFont() const;
	[[nodiscard]] TTF_Font *GetTTFFont() const;

};

class LinkFont
{
private:
	friend class AssetsClass;
	std::string m_Key;
	std::string m_FontID;
	int m_Size;
	Font *m_Font;

public:
	explicit LinkFont(std::string font_key);

	// Getting
	[[nodiscard]] const std::string& Key() const { return m_Key; }
	[[nodiscard]] const std::string& FontID() const { return m_FontID; }
	[[nodiscard]] int Size() const { return m_Size; }
	[[nodiscard]] Font *GetFont() const;

};