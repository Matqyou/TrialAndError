//
// Created by Mat on 12/3/2024.
//

#include "Assets.h"
#include <client/core/Application.h>
#include <unordered_set>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <utility>
#include <cassert>

using namespace Strings;

std::vector<LoadTexture *> AssetsClass::m_LoadTextures = { };
//std::vector<PregenerateTexture *> AssetsClass::m_PregenerateTextures = { };
std::vector<LinkSound *> AssetsClass::m_LinkSounds = { };
std::vector<PreloadFont *> AssetsClass::m_PreloadFonts = { };
std::vector<LinkFont *> AssetsClass::m_LinkFonts = { };

// Iterators
//std::vector<LoadTexture *>::iterator AssetsClass::m_LinkTexturesIterator = std::vector<LoadTexture *>::iterator{ };
//std::vector<LinkSound *>::iterator AssetsClass::m_LinkSoundsIterator = std::vector<LinkSound *>::iterator{ };
//std::vector<LinkFont *>::iterator AssetsClass::m_LinkFontsIterator = std::vector<LinkFont *>::iterator{ };
//std::vector<PregenerateTexture *>::iterator
//	AssetsClass::m_PregenerateTexturesIterator = std::vector<PregenerateTexture *>::iterator{ };

size_t AssetsClass::sTotalWork = 0;
size_t AssetsClass::sWorkDone = 0;

Sound::Sound(std::string key, MIX_Audio *init_mix_audio, std::string load_extension)
	: m_Key(std::move(key)),
	  m_LoadExtension(std::move(load_extension))
{
	mix_audio = init_mix_audio;
}

Sound::~Sound()
{
	if (mix_audio)
		MIX_DestroyAudio(mix_audio);
	mix_audio = nullptr;
}

void Sound::SetVolume(int volume)
{
	// todo: couldn't find the volume function lawl
//	MIX_Audio
//	Mix_VolumeChunk(mix_audio, volume);
}

void Sound::PlaySound()
{
	if (mix_audio == nullptr || !Assets.SoundsEnabled())
		return;

//	dbg_msg("Setting audio on new track %p, audio %p\n", (void*)Application.GetTrack(), (void*)mix_audio);

	// todo: change for tracks
	MIX_SetTrackAudio(Application.GetTrack(), mix_audio);
	MIX_PlayTrack(Application.GetTrack(), 0);
//	MIX_PlayAudio();
//	Mix_PlayChannel(-1, mix_audio, 0);
}

Font::Font(TTF_Font *ttf_font, std::string key, std::string load_extension)
	: m_Key(std::move(key)),
	  m_LoadExtension(std::move(load_extension))
{
	m_TTFFont = ttf_font;
	m_Size = -1.0f;
}

Font::~Font()
{
	if (m_TTFFont)
		TTF_CloseFont(m_TTFFont);
}

std::vector<std::tuple<std::string, std::string, std::string>> GetResourceKeys(
	const char *directory,
	const std::unordered_set<std::string>& supported_extensions
)
{
	std::vector<std::tuple<std::string, std::string, std::string>> results;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
	{
		if (!entry.is_regular_file())
			continue;

		const std::filesystem::path& file_path = entry.path();

		// Extension in lowercase
		const std::string extension = file_path.extension().string();
		std::string extension_lower(extension);
		std::transform(extension_lower.begin(), extension_lower.end(), extension_lower.begin(), ::tolower);

		// Process the path string for logging or other use
		std::string identificator = file_path.string();
		identificator = ErasePrefix(identificator, directory);
		identificator = EraseSuffix(identificator, extension);

		// Convert to lowercase and replace slashes to periods
		std::transform(identificator.begin(), identificator.end(), identificator.begin(), ::tolower);
		std::replace(identificator.begin(), identificator.end(), '/', '.');
		std::replace(identificator.begin(), identificator.end(), '\\', '.');

		// Check if the extension is supported
		if (supported_extensions.find(extension) == supported_extensions.end())
		{
//			dbg_msg("[Assets] &8Unsupported file format '%s' for '%s'\n", extension.c_str(), identificator.c_str());
			continue;
		}

		// Add the key and path to the results
		results.emplace_back(identificator, file_path.string(), extension);
	}

	return results;
}

// AssetsClass start

bool AssetsClass::LoadingSurfaces()
{
	if (m_SurfaceResourcesIndex >= m_SurfaceResources.size())
		return false;

	sWorkDone++;

	auto& entry = m_SurfaceResources[m_SurfaceResourcesIndex];
	++m_SurfaceResourcesIndex;
	std::string& surface_key = std::get<0>(entry);
	std::string& file_path = std::get<1>(entry);
	std::string& extension = std::get<2>(entry);

	// Look for duplicate names
	auto duplicate_iterator = m_Surfaces.find(surface_key);
	if (duplicate_iterator != m_Surfaces.end())
	{
		failed_load_surfaces.push_back(Strings::FString("[Assets] &eCACHE &cDuplicate surface '%s' &8(rename one of the files)\n", extension.c_str(), surface_key.c_str()));
		return true;
	}

	// Load the surface
	SDL_Surface *sdl_surface = IMG_Load(file_path.c_str());
	if (sdl_surface == nullptr)
	{
		failed_load_surfaces.push_back(Strings::FString("[Assets] &eDISK &cFailed to load texture '%s'(%s)\n", file_path.c_str(), SDL_GetError()));
		return true;
	}

	// Check if texture has a visual hitbox file
	auto visual_hitbox_iterator = std::find_if(m_VisHitboxResources.begin(), m_VisHitboxResources.end(),
											   [&surface_key](const std::tuple<std::string, std::string, std::string>& item)
											   {
												   return std::get<0>(item) == surface_key; // Compare first string
											   });

	Surface *new_surface = nullptr;
	bool has_hitbox_file = visual_hitbox_iterator != m_VisHitboxResources.end();
	if (has_hitbox_file)
	{
		HitboxFile visual_hitbox;
		if (!visual_hitbox.OpenFile(R"(.\assets\images\)", surface_key))
		{
			failed_load_surfaces.push_back(Strings::FString("[Assets] &eDISK &cFailed to load visual hitbox file '%s'\n", std::get<
				1>(*visual_hitbox_iterator).c_str()));
			return true;
		}
		else
			// Convert to texture
			new_surface = new VisualSurface(sdl_surface, Rect4f(visual_hitbox.Hitbox()));
	}
	else
		new_surface = new Surface(sdl_surface);

//	new_surface->FlagForAutomaticDeletion();
	// Add it to all the textures
	m_Surfaces[surface_key] = new_surface;
	dbg_msg("[Assets] &aDISK &9Loaded surface '%s'\n", surface_key.c_str());
	success_load_surfaces++;

	// Last
	if (m_SurfaceResourcesIndex == m_SurfaceResources.size())
		m_InvalidSurfaceDefault = GetSurface("invalid");
	return true;
}

bool AssetsClass::LoadingSounds()
{
	if (m_SoundResourcesIndex >= m_SoundResources.size())
		return false;

	sWorkDone++;

	auto& entry = m_SoundResources[m_SoundResourcesIndex];
	++m_SoundResourcesIndex;
	std::string& key = std::get<0>(entry);
	std::string& file_path = std::get<1>(entry);
	std::string& extension = std::get<2>(entry);

	auto it = m_Sounds.find(key);
	if (it != m_Sounds.end())
	{
		failed_load_sounds.push_back(Strings::FStringColors("[Assets] &aCACHE &cDuplicate sound '%s' for existing '%s'(%s)\n", extension.c_str(), key.c_str(), it->second->m_LoadExtension.c_str()));
		return true;
	}

	// Load the sound
	MIX_Audio *new_mix_audio = MIX_LoadAudio(Application.GetMixer(), file_path.c_str(), true);
	if (!new_mix_audio)
	{
		failed_load_sounds.push_back(Strings::FStringColors("[Assets] &aDISK &cFailed to load sound '%s'(%s)\n", file_path.c_str(), SDL_GetError()));
		return true;
	}

	// Add it to all the textures
	auto new_sound = new Sound(key, new_mix_audio, extension);
	m_Sounds[key] = new_sound;
	dbg_msg("[Assets] &aDISK &9Loaded sound '%s'\n", key.c_str());
	success_load_sounds++;

	return true;
}

bool AssetsClass::LoadingFonts()
{
	if (m_PreloadFontIterator == m_PreloadFonts.end())
		return false;

	sWorkDone++;

	auto required_font = *m_PreloadFontIterator;
	++m_PreloadFontIterator;
	const std::string& font_key = required_font->Key();
	const std::string& font_id = required_font->FontID();
	float font_size = required_font->Size();

	auto it = m_Fonts.find(font_key);
	if (it != m_Fonts.end())
	{
		auto [other_font_id, other_font] = *it;
		failed_load_fonts.push_back(
			FString(
				"[Assets] &aCACHE &cDuplicate font '%s'&r(%s:%.1fpt), existing font '%s'&r(%s:%.1fpt)\n",
				font_key.c_str(),
				font_id.c_str(),
				font_size,
				other_font->m_Key.c_str(),
				other_font_id.c_str(),
				other_font->m_Size
			)
		);
		return true;
	}

	for (auto entry : m_FontResources)
	{
		std::string& id = std::get<0>(entry);
		std::string& file_path = std::get<1>(entry);
		std::string& extension = std::get<2>(entry);

		if (font_id == id)
		{
			auto ttf_font = TTF_OpenFont(file_path.c_str(), font_size);
			if (!ttf_font)
			{
				failed_load_fonts.push_back(FString("[Assets] &aDISK &cFailed to load font '%s' because (%s)", file_path.c_str(), SDL_GetError()));
				continue;
			}

			auto new_font = (new Font(ttf_font, font_key, extension))
				->CacheSize(font_size);
			required_font->m_Font = new_font;
			m_Fonts[font_key] = new_font;
			dbg_msg("[Assets] &aDISK &9Loaded font '%s' &7(%s:%.1fpt)\n",
					font_key.c_str(),
					font_id.c_str(),
					font_size);
			success_load_fonts++;
		}
	}

	return true;
}

//bool AssetsClass::GeneratingTextures()
//{
//	if (m_PregenerateTexturesIterator == m_PregenerateTextures.end())
//		return false;
//
//	sWorkDone++;
//
//	auto generate_texture = *m_PregenerateTexturesIterator;
//	++m_PregenerateTexturesIterator;
//	const std::string& texture_key = generate_texture->Key();
//
//	// Find if a texture with the same key exists
//	auto it = m_Textures.find(texture_key);
//	if (it != m_Textures.end())
//	{
//		dbg_msg("[Assets] &cDuplicate pre-generated texture '%s'\n", texture_key.c_str());
//		return true;
//	}
//
//	// If no callback was set, no generation can take place
//	if (!generate_texture->m_GenerateCallback)
//	{
//		dbg_msg("[Assets] &cCould not pre-generate '%s', invalid callback\n", texture_key.c_str());
//		return true;
//	}
//
//	// If generator returned nullptr, there must've been an error
//	auto new_texture = generate_texture->m_GenerateCallback(this);
//	if (new_texture == nullptr)
//	{
//		dbg_msg("[Assets] &cCould not pre-generate '%s', invalid texture\n", texture_key.c_str());
//		return true;
//	}
//
//	new_texture->FlagForAutomaticDeletion();
//	generate_texture->m_Texture = new_texture;
//	m_Textures[texture_key] = new_texture;
//	dbg_msg("[Assets] &eGenerated texture '%s'\n", texture_key.c_str());
//
//	return true;
//}

bool AssetsClass::LoadingTextures()
{
	if (m_LoadTextures.empty())
		return false; // todo: bad

	sWorkDone += m_LoadTextures.size();
	m_InvalidTextureDefault = TextureFromSurface(*GetSurface("invalid"), TexturePurpose::GUI_ELEMENT);

	for (LoadTexture *load_texture : m_LoadTextures)
	{
		const std::string& texture_key = load_texture->Key();
		const std::string& from_surface_key = load_texture->FromSurfaceKey();
		TexturePurpose purpose = load_texture->GetPurpose();

		auto duplicate_iterator = m_Textures.find(texture_key);
		if (duplicate_iterator != m_Textures.end())
		{
			failed_load_textures.push_back(Strings::FStringColors("[Assets] &eCACHE &cDuplicate texture '%s' &8(remove one, reuse the other for your use cases)\n", texture_key.c_str()));
			continue;
		}

		Surface *from_surface = GetSurface(from_surface_key);
		if (!from_surface)
		{
			failed_load_textures.push_back(Strings::FStringColors("[Assets] &eCACHE &cFailed to load texture '%s', surface '%s' not found\n", texture_key.c_str(), from_surface_key.c_str()));
			continue;
		}

		Texture *new_texture = TextureFromSurface(*from_surface, purpose);
		load_texture->texture = new_texture;

		m_Textures[texture_key] = new_texture;
		dbg_msg("[Assets] &aRAM &9Loaded texture '%s'\n", texture_key.c_str());
		success_load_textures++;
	}
//	dbg_msg("[Assets] &eLoaded %zu/%zu textures\n", m_Textures.size(), m_LoadTextures.size());
	m_LoadTextures.clear();

	return true;
}

bool AssetsClass::LinkingSounds()
{
	if (m_LinkSounds.empty())
		return false; // todo: bad

	sWorkDone += m_LinkSounds.size();

//	dbg_msg("[Assets] &eLoaded %i sounds\n", m_Sounds.size());

	// Link
	for (auto required_sound : m_LinkSounds)
	{
		const std::string& sound_key = required_sound->Key();

		auto sound = GetSound(sound_key);
		if (!sound)
		{
			failed_link_sounds.push_back(Strings::FStringColors("[Assets] &eCACHE &cFailed to link missing sound '%s'\n", sound_key.c_str()));
			continue;
		}

		required_sound->m_Sound = sound;
		dbg_msg("[Assets] &aCACHE &9Linked sound '%s'\n", sound_key.c_str());
		success_link_sounds++;
	}
//	dbg_msg("[Assets] &eLinked %zu sounds\n", m_LinkSounds.size());
	m_LinkSounds.clear();

	return true;
}

bool AssetsClass::LinkingFonts()
{
	if (m_LinkFonts.empty())
		return false; // todo: very bad

	sWorkDone += m_LinkFonts.size();

	for (auto required_font : m_LinkFonts)
	{
		const std::string& font_key = required_font->Key();

		auto font = GetFont(font_key);
		if (!font)
		{
			failed_link_fonts.push_back(Strings::FString("[Assets] &eCACHE &cFailed to link missing font '%s'\n", font_key.c_str()));
			continue;
		}

		required_font->m_Font = font;
		dbg_msg("[Assets] &aCACHE &9Linked font '%s'\n", font_key.c_str());
		success_link_fonts++;
	}

//	auto loaded_fonts_message = FString("[Assets] &5Loaded %zu fonts", m_PreloadFonts.size());
//	if (!m_LinkFonts.empty()) loaded_fonts_message += FString(" &d(%zu linked)", m_LinkFonts.size());
//	dbg_msg("%ls\n", loaded_fonts_message.c_str());

	m_LinkFonts.clear();
	return true;
}

int CalculateMipmapLevels(int width, int height)
{
	int maxDimension = std::max(width, height);
	return static_cast<int>(std::floor(std::log2(maxDimension))) + 1;
}

SDL_GPUTextureCreateInfo AssetsClass::GetGPUTextureCreateInfo(const Vec2i& texture_size, TexturePurpose purpose)
{
	SDL_GPUTextureCreateInfo texture_info = { };
	texture_info.width = texture_size.x;
	texture_info.height = texture_size.y;
	texture_info.sample_count = SDL_GPU_SAMPLECOUNT_1;

	switch (purpose)
	{
		case TexturePurpose::GUI_ELEMENT:
		{
			texture_info.type = SDL_GPU_TEXTURETYPE_2D;
			texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
			texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
			texture_info.layer_count_or_depth = 1;
			texture_info.num_levels = 1;  // No mipmaps
			break;
		}
		case TexturePurpose::OBJECT_3D:
		{
			texture_info.type = SDL_GPU_TEXTURETYPE_2D;
			texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
			texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
			texture_info.layer_count_or_depth = 1;
			texture_info.num_levels = CalculateMipmapLevels(texture_size.x, texture_size.y);  // Full mipmap chain
			break;
		}
		case TexturePurpose::RENDER_TARGET:
		{
			texture_info.type = SDL_GPU_TEXTURETYPE_2D;
			texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
			texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
			texture_info.layer_count_or_depth = 1;
			texture_info.num_levels = 1;
			break;
		}
		case TexturePurpose::DEPTH_BUFFER:
		{
			texture_info.type = SDL_GPU_TEXTURETYPE_2D;
			texture_info.format = SDL_GPU_TEXTUREFORMAT_D24_UNORM;
			texture_info.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
			texture_info.layer_count_or_depth = 1;
			texture_info.num_levels = 1;
			break;
		}
		case TexturePurpose::CUBEMAP:
		{
			texture_info.type = SDL_GPU_TEXTURETYPE_CUBE;
			texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
			texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
			texture_info.layer_count_or_depth = 6;  // 6 faces
			texture_info.num_levels = CalculateMipmapLevels(texture_size.x, texture_size.y);
			break;
		}
		case TexturePurpose::TEXTURE_ATLAS:
		{
			texture_info.type = SDL_GPU_TEXTURETYPE_2D;
			texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
			texture_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;
			texture_info.layer_count_or_depth = 1;
			texture_info.num_levels = 1;  // Usually no mipmaps for atlases
			break;
		}
	}

	return texture_info;
}

SDL_GPUTexture *AssetsClass::GPUTextureFromSurface(SDL_Surface *sdl_surface, TexturePurpose purpose)
{
	// 1. Convert surface to RGBA8888 if needed
	SDL_Surface *rgba_surface = SDL_ConvertSurface(sdl_surface, SDL_PIXELFORMAT_RGBA32);
	if (!rgba_surface)
	{
		printf("ERROR: Failed to convert surface: %s\n", SDL_GetError());
		return nullptr;
	}

	// 2. Create GPU texture
//	SDL_GPUTextureCreateInfo texture_info = {
//		.type = SDL_GPU_TEXTURETYPE_2D,
//		.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
//		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
//		.width = (Uint32)rgba_surface->w,
//		.height = (Uint32)rgba_surface->h,
//		.layer_count_or_depth = 1,
//		.num_levels = 1
//	};
	SDL_GPUTextureCreateInfo texture_info = GetGPUTextureCreateInfo(Vec2i(sdl_surface->w, sdl_surface->h), purpose);
	SDL_GPUTexture *gpu_texture = SDL_CreateGPUTexture(Drawing.Device(), &texture_info);

	// 3. Create transfer buffer
	Uint32 data_size = rgba_surface->w * rgba_surface->h * 4;
	SDL_GPUTransferBufferCreateInfo transfer_info = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = data_size
	};
	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(Drawing.Device(), &transfer_info);

	// 4. Copy surface pixels to transfer buffer
	void *mapped = SDL_MapGPUTransferBuffer(Drawing.Device(), transfer, false);
	memcpy(mapped, rgba_surface->pixels, data_size);
	SDL_UnmapGPUTransferBuffer(Drawing.Device(), transfer);

	// 5. Upload to GPU
	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(Drawing.Device());
	SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(cmd);

	SDL_GPUTextureTransferInfo src = {
		.transfer_buffer = transfer,
		.offset = 0
	};
	SDL_GPUTextureRegion dst = {
		.texture = gpu_texture,
		.w = (Uint32)rgba_surface->w,
		.h = (Uint32)rgba_surface->h,
		.d = 1
	};
	SDL_UploadToGPUTexture(copy_pass, &src, &dst, false);

	SDL_EndGPUCopyPass(copy_pass);
	SDL_SubmitGPUCommandBuffer(cmd);

	// 6. Cleanup
	SDL_ReleaseGPUTransferBuffer(Drawing.Device(), transfer);
	SDL_DestroySurface(rgba_surface);

	return gpu_texture;
}

AssetsClass::AssetsClass()
	: m_InvalidSurfaceDefault(nullptr),
	  m_InvalidTextureDefault(nullptr)
{
	status = Status::UNINITIALIZED;

	m_SoundsEnabled = false;
	sTotalWork = 0;

	success_load_surfaces = 0;
	success_load_textures = 0;
	success_load_sounds = 0;
	success_load_fonts = 0;
	success_link_sounds = 0;
	success_link_fonts = 0;
}

// Todo: Loading assets in realtime (adding/removing files?)
void AssetsClass::Initialize(bool sounds_enabled)
{
	// This function must contain a status override
	if (status != Status::UNINITIALIZED)
		return;

	m_SoundsEnabled = sounds_enabled;

	dbg_msg("[Assets] &8Looking for something interesting..\n");
	std::unordered_set<std::string> hitbox_extension = { ".vis" };
	std::unordered_set<std::string> texture_extensions = { ".png", ".jpg", ".jpeg", ".bmp",
														   ".gif", ".tif", ".tiff", ".webp" };
	// ".wav" doesn't work for some reason
	std::unordered_set<std::string> sound_extensions = { ".aiff", ".voc", ".mp3", ".ogg", ".flac", ".mod",
														 ".s3m", ".it", ".xm", ".mid", ".midi", ".opus", ".wav" };
	std::unordered_set<std::string> font_extensions = { ".ttf", ".otf", ".fon", ".fnt",
														".bdf", ".pcf", ".sfnt", ".ttc" };
	m_VisHitboxResources = GetResourceKeys(R"(.\assets\images\)", hitbox_extension);
	m_SurfaceResources = GetResourceKeys(R"(.\assets\images\)", texture_extensions);
	m_SoundResources = GetResourceKeys(R"(.\assets\sounds\)", sound_extensions);
	m_FontResources = GetResourceKeys(R"(.\assets\fonts\)", font_extensions);

	status = Status::KEYS_LOADED;
	m_SurfaceResourcesIndex = 0;
	m_SoundResourcesIndex = 0;
//	m_TextureResourcesIterator = m_TextureResources.begin();
//	m_SoundResourcesIterator = m_SoundResources.begin();
	m_PreloadFontIterator = m_PreloadFonts.begin();
//	m_PregenerateTexturesIterator = m_PregenerateTextures.begin();
//	m_LinkTexturesIterator = m_LoadTextures.begin();
//	m_LinkSoundsIterator = m_LinkSounds.begin();
//	m_LinkFontsIterator = m_LinkFonts.begin();

	dbg_msg("[Assets] &aDISK &2Found %zu textures in assets\n", m_SurfaceResources.size());
	dbg_msg("[Assets] &aDISK &2Found %zu sounds in assets\n", m_SoundResources.size());
	dbg_msg("[Assets] &aDISK &2Found %zu fonts in assets\n", m_FontResources.size());
	dbg_msg("[Assets] &aCACHE &2Found %zu font loads\n", m_PreloadFonts.size());
	dbg_msg("[Assets] &aCACHE &2Found %zu texture loads\n", m_LoadTextures.size());
	dbg_msg("[Assets] &aCACHE &2Found %zu sound links\n", m_LinkSounds.size());
	dbg_msg("[Assets] &aCACHE &2Found %zu font links\n", m_LinkFonts.size());

	sTotalWork = m_SurfaceResources.size() + m_SoundResources.size() + m_PreloadFonts.size() // m_MusicResources.size()
		+ m_LoadTextures.size() + m_LinkSounds.size() + m_LinkFonts.size();

	dbg_msg("[Assets] &8Proceeding with loading..\n"); // now onto 'threaded loading'
}

void AssetsClass::Destroy()
{
	if (status == Status::UNINITIALIZED)
		return;

	status = Status::UNINITIALIZED;

	size_t destroyed_surfaces = 0;
	auto surface_it = cleanup_surfaces.begin();
	while (surface_it != cleanup_surfaces.end())
	{
		Surface *surface = *surface_it;
		surface_it = cleanup_surfaces.erase(surface_it);
		destroyed_surfaces += surface->Destroy();
	}
	cleanup_surfaces.clear();

	size_t destroyed_textures = 0;
	auto texture_it = cleanup_textures.begin();
	while (texture_it != cleanup_textures.end())
	{
		Texture *texture = *texture_it;
		texture_it = cleanup_textures.erase(texture_it);
		destroyed_textures += texture->Destroy();
	}
	cleanup_textures.clear();

	size_t destroyed_sounds = 0;
	for (const auto& entry : m_Sounds)
	{
		delete entry.second;
		destroyed_sounds++;
	}
	m_Sounds.clear();

	size_t destroyed_fonts = 0;
	for (const auto& entry : m_Fonts)
	{
		delete entry.second;
		destroyed_fonts++;
	}
	m_Fonts.clear();

	size_t preloaded_surfaces = m_Surfaces.size(); // Also deleted here
	auto other_destroyed_surfaces = destroyed_surfaces - preloaded_surfaces;
	m_Surfaces.clear();

	auto destroyed_surfaces_msg = FString("[Assets] &5Destroyed %zu surfaces", preloaded_surfaces);
	if (other_destroyed_surfaces > 0)
		destroyed_surfaces_msg += FString(" &d(+%zu dynamic)", other_destroyed_surfaces);

	size_t preloaded_textures = m_Textures.size(); // Also deleted here
	auto other_destroyed_textures = destroyed_textures - preloaded_textures;
	m_Textures.clear();

	auto destroyed_textures_msg = FString("[Assets] &5Destroyed %zu textures", preloaded_textures);
	if (other_destroyed_textures > 0)
		destroyed_textures_msg += FString(" &d(+%zu dynamic)", other_destroyed_textures);

	dbg_msg("%s\n", destroyed_surfaces_msg.c_str());
	dbg_msg("%s\n", destroyed_textures_msg.c_str());
	dbg_msg("[Assets] &5Destroyed %zu sounds\n", destroyed_sounds);
	dbg_msg("[Assets] &5Destroyed %zu fonts\n", destroyed_fonts);
}

AssetsClass::~AssetsClass()
{
	Destroy();
}

Surface *AssetsClass::GetSurface(const std::string& surface_key)
{
	auto it = m_Surfaces.find(surface_key);
	if (it != m_Surfaces.end())
		return it->second;

	return nullptr;

//	if (m_InvalidSurfaceDefault != nullptr)
//	{
//		dbg_msg("[Assets] GetSurface() `%s` not found\n", surface_key.c_str());
//		return m_InvalidSurfaceDefault;
//	}
//
//	throw std::runtime_error(
//		Strings::FString(
//			"AssetsClass::GetSurface() surface '%s' was not found (no invalid surface found either)\n",
//			surface_key.c_str()
//		)
//	);
}

Texture *AssetsClass::GetTexture(const std::string& texture_key)
{
	auto it = m_Textures.find(texture_key);
	if (it != m_Textures.end())
		return it->second;

	if (m_InvalidTextureDefault != nullptr)
		return m_InvalidTextureDefault;

	throw std::runtime_error(
		Strings::FString(
			"Assets.h GetTexture() texture '%s' was not found (no invalid texture found either)\n",
			texture_key.c_str()
		)
	);
}

const std::unordered_map<std::string, Texture *>& AssetsClass::GetAllTextures()
{
	return m_Textures;
}

Sound *AssetsClass::GetSound(const std::string& sound_key)
{
	auto it = m_Sounds.find(sound_key);
	if (it != m_Sounds.end())
		return it->second;

	return nullptr;
}

Font *AssetsClass::GetFont(const std::string& font_key)
{
	auto it = m_Fonts.find(font_key);
	if (it != m_Fonts.end())
		return it->second;

	return nullptr;
}

Texture *AssetsClass::TextureFromSurface(const Surface& surface, TexturePurpose purpose)
{
	Vec2i texture_size = surface.GetSize();
	SDL_Surface *sdl_surface = surface.SDLSurface();
	if (surface.UsesHitbox())
	{
		VisualSurface *visual_surface = (VisualSurface *)&surface;
		return new VisualTexture(surface.GetSize(), GPUTextureFromSurface(sdl_surface, purpose), visual_surface->GetHitbox());
	}

	return new Texture(texture_size, GPUTextureFromSurface(sdl_surface, purpose));
}

SDL_Surface *AssetsClass::CreateSDLSurface(int width, int height, SDL_PixelFormat format)
{
	SDL_Surface *sdl_surface = SDL_CreateSurface(width, height, format);
	return sdl_surface;
}

Texture *AssetsClass::CreateTexture(TexturePurpose purpose, const Vec2i& size, Rect4f *visual_hitbox)
{
	auto texture_info = GetGPUTextureCreateInfo(size, purpose);
	SDL_GPUTexture *gpu_texture = SDL_CreateGPUTexture(Drawing.Device(), &texture_info);
	if (visual_hitbox)
		return new VisualTexture(size, gpu_texture, *visual_hitbox);

	return new Texture(size, gpu_texture);
}

Texture *AssetsClass::RenderTextBlended(TTF_Font *font, const std::string& text, SDL_Color color)
{
	Surface temp_surface(TTF_RenderText_Blended(font, text.c_str(), text.size(), color));
	Texture *text_render = TextureFromSurface(temp_surface, TexturePurpose::GUI_ELEMENT);

	return text_render;
}

Texture *AssetsClass::RenderTextBlended(TTF_Font *font, const char *text, SDL_Color color)
{
	Surface temp_surface(TTF_RenderText_Blended(font, text, strlen(text), color));
	Texture *text_render = TextureFromSurface(temp_surface, TexturePurpose::GUI_ELEMENT);

	return text_render;
}

//Texture *AssetsClass::RenderTextBlendedOutline(TTF_Font *font, const char *text, int thickness,
//											   SDL_Color text_color, SDL_Color outline_color)
//{
//	auto original_render_target = SDL_GetRenderTarget(Application.GetDrawing()->Renderer());
//
//	TTF_SetFontOutline(font, thickness);
//	SDL_Surface *outlineSurface = TTF_RenderText_Blended(font, text, strlen(text),
//														 { outline_color.r, outline_color.g, outline_color.b, 255 });
//
//	TTF_SetFontOutline(font, 0);
//
//	SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, strlen(text), text_color);
//
//	SDL_Texture *outlineTexture = SDL_CreateTextureFromSurface(Application.GetDrawing()->Renderer(), outlineSurface);
//	SDL_SetTextureAlphaMod(outlineTexture, outline_color.a);
//	SDL_Texture *textTexture = SDL_CreateTextureFromSurface(Application.GetDrawing()->Renderer(), textSurface);
//
//	auto result = SDL_CreateTexture(Application.GetDrawing()->Renderer(),
//									SDL_PIXELFORMAT_RGBA8888,
//									SDL_TEXTUREACCESS_TARGET,
//									outlineTexture->w, outlineTexture->h);
//	Application.GetDrawing()->SetRenderTargetSDL(result);
//	Application.GetDrawing()->RenderTextureFullscreen(outlineTexture, nullptr);
//	SDL_FRect text_destination = {
//		(float)thickness, (float)thickness,
//		(float)textTexture->w, (float)textTexture->h
//	};
//	Application.GetDrawing()->RenderTexture(textTexture, nullptr, text_destination);
//	Application.GetDrawing()->SetRenderTargetSDL(original_render_target);
//
//	SDL_DestroyTexture(outlineTexture);
//	SDL_DestroyTexture(textTexture);
//
//	return new VisualTexture(result, Rect4f(text_destination.x, text_destination.y, text_destination.w, text_destination.h));
//}

Texture *AssetsClass::RenderTextSolid(TTF_Font *font, const std::string& text, SDL_Color color)
{
	Surface temp_surface(TTF_RenderText_Solid(font, text.c_str(), text.size(), color));
	Texture *text_render = TextureFromSurface(temp_surface, TexturePurpose::GUI_ELEMENT);

	return text_render;
}

Texture *AssetsClass::RenderTextSolid(TTF_Font *font, const char *text, SDL_Color color)
{
	Surface temp_surface(TTF_RenderText_Solid(font, text, strlen(text), color));
	Texture *text_render = TextureFromSurface(temp_surface, TexturePurpose::GUI_ELEMENT);

	return text_render;
}

//Texture *AssetsClass::RenderTextureOnTextureCentered(Texture *outer, Texture *inner, bool copy)
//{
//	auto original_render_target = SDL_GetRenderTarget(Application.GetDrawing()->Renderer());
//
//	auto result_texture = copy ? outer->CopyTexture(SDL_TEXTUREACCESS_TARGET) : outer;
//	auto outer_size = outer->GetSize();
//	auto inner_size = inner->GetSize();
//	auto position = Rectangles::CenterRelative(inner_size, outer_size);
//	Application.GetDrawing()->SetRenderTarget(result_texture);
//	Application.GetDrawing()->RenderTexture(inner->SDLTexture(), nullptr, position);
//	Application.GetDrawing()->SetRenderTargetSDL(original_render_target);
//	return result_texture;
//}
//
//bool AssetsClass::SaveTextureToDisk(Texture *texture, const std::string& filename)
//{
//	auto original_render_target = SDL_GetRenderTarget(Application.GetDrawing()->Renderer());
//
//	Application.GetDrawing()->SetRenderTarget(texture);
//	SDL_Surface *sdl_surface = SDL_RenderReadPixels(Application.GetDrawing()->Renderer(), nullptr);
//	Application.GetDrawing()->SetRenderTargetSDL(original_render_target);
//
//	bool save_result = IMG_SavePNG(sdl_surface, filename.c_str());
//	SDL_DestroySurface(sdl_surface);
//	if (!save_result)
//	{
//		dbg_msg("[Assets] &cFailed to export texture to disk\n");
//		dbg_msg("[Assets] &cReason: %s\n", SDL_GetError());
//		return false;
//	}
//
//	dbg_msg("[Assets] &aExported texture to disk as %s\n", filename.c_str());
//	return true;
//}

void AssetsClass::NewSurface(Surface *new_surface)
{
	cleanup_surfaces.insert(new_surface);
}

void AssetsClass::RemoveSurface(Surface *remove_surface)
{
	cleanup_surfaces.erase(remove_surface);
}

void AssetsClass::NewTexture(Texture *new_texture)
{
	cleanup_textures.insert(new_texture);
}

void AssetsClass::RemoveTexture(Texture *remove_texture)
{
	cleanup_textures.erase(remove_texture);
}

void AssetsClass::AddLoadTexture(LoadTexture *load_texture)
{
	if (Assets.GetStatus() < Status::ASSETS_LINKED)
	{
		m_LoadTextures.push_back(load_texture);
		return;
	}

	auto error = Strings::FString("You cannot load texture '%s' after status == ASSETS_LINKED", load_texture->key.c_str());
	throw std::runtime_error(error);
}

//void AssetsClass::LinkPregeneratedTexture(PregenerateTexture *pregenerate_texture)
//{
//	if (Assets.GetStatus() < Status::ASSETS_LINKED)
//	{
//		m_PregenerateTextures.push_back(pregenerate_texture);
//		return;
//	}
//
//	auto error = Strings::FString("You cannot pre-generate texture '%s' after status == ASSETS_LINKED", pregenerate_texture->m_Key.c_str());
//	throw std::runtime_error(error);
//}

void AssetsClass::LinkPreloadedSound(LinkSound *link_sound)
{
	if (Assets.GetStatus() < Status::ASSETS_LINKED)
	{
		m_LinkSounds.push_back(link_sound);
		return;
	}

	auto error = Strings::FString("You cannot link sound '%s' after status == ASSETS_LINKED", link_sound->m_Key.c_str());
	throw std::runtime_error(error);
}

void AssetsClass::PreloadFont_(PreloadFont *preload_font)
{
	if (Assets.GetStatus() < Status::ASSETS_LINKED)
	{
		m_PreloadFonts.push_back(preload_font);
		return;
	}

	auto error = Strings::FString("You cannot pre-load font '%s' after status == ASSETS_LINKED", preload_font->m_Key.c_str());
	throw std::runtime_error(error);
}

void AssetsClass::LinkPreloadedFont(LinkFont *link_font)
{
	if (Assets.GetStatus() < Status::ASSETS_LINKED)
	{
		m_LinkFonts.push_back(link_font);
		return;
	}

	auto error = Strings::FString("You cannot link font '%s' after status == ASSETS_LINKED",
								  link_font->m_Key.c_str());
	throw std::runtime_error(error);
}

void AssetsClass::ThreadedLoading()
{
	while (status == Status::KEYS_LOADED)
	{
		if (LoadingSurfaces()) break; // Only loads
		if (LoadingSounds()) break; // Only loads
		if (LoadingFonts()) break; // Loads and links
		if (LoadingTextures()) break; // Loads and links
		if (LinkingSounds()) break; // Only links
		if (LinkingFonts()) break; // Only links

		status = Status::ASSETS_LINKED;
		dbg_msg("[Assets] &8Finished loading!\n");

		// %s prefix
		// %zu successful
		// %zu total
		// %s suffix message
		auto summary = [](const char *fmt, size_t success, size_t fail)
		{
			size_t total = success + fail;
			std::string color = (total == 0) ? "&8" : (success == total) ? "&a" : "&e";
			std::string prefix = color + ((success == total) ? "✔ " : "✖ ");
			std::string suffix = (success == total) ? "" :
								 (success != total) ? Strings::FString(" &f.. &c%zu failed :/", fail) :
								 " &f.. &call failed ;-;";
			dbg_msg(fmt, prefix.c_str(), success, total, suffix.c_str());
		};

		size_t total_problems = failed_load_surfaces.size() +
			failed_load_textures.size() +
			failed_load_sounds.size() +
			failed_load_fonts.size() +
			failed_link_sounds.size() +
			failed_link_fonts.size();
		if (total_problems)
		{
			dbg_msg("\n");
			dbg_msg("[Assets] &cFound %zu problem/s during loading:\n", total_problems);
			for (auto msg : failed_load_surfaces) dbg_msg(msg.c_str());
			for (auto msg : failed_load_textures) dbg_msg(msg.c_str());
			for (auto msg : failed_load_sounds) dbg_msg(msg.c_str());
			for (auto msg : failed_load_fonts) dbg_msg(msg.c_str());
			for (auto msg : failed_link_sounds) dbg_msg(msg.c_str());
			for (auto msg : failed_link_fonts) dbg_msg(msg.c_str());
			dbg_msg("\n");
		}

		summary("[Assets] %sLoaded %zu/%zu surfaces%s\n", success_load_surfaces, failed_load_surfaces.size());
		summary("[Assets] %sLoaded %zu/%zu textures%s\n", success_load_textures, failed_load_textures.size());
		summary("[Assets] %sLoaded %zu/%zu sounds%s\n", success_load_sounds, failed_load_sounds.size());
		summary("[Assets] %sLoaded %zu/%zu fonts%s\n", success_load_fonts, failed_load_fonts.size());
		summary("[Assets] %sLinked %zu/%zu sounds%s\n", success_link_sounds, failed_link_sounds.size());
		summary("[Assets] %sLinked %zu/%zu fonts%s\n", success_link_fonts, failed_link_fonts.size());
		summary("[Assets] %sProcessed entries %zu/%zu%s\n", GetWorkDone(), GetTotalWork() - GetWorkDone());
	}
}

AssetsClass Assets;
// AssetsClass end

LoadTexture::LoadTexture(std::string new_key, AssetsClass::TexturePurpose purpose)
	: key(new_key)
{
	texture = nullptr;
	from_surface_key = key;
	texture_purpose = purpose;

	AssetsClass::AddLoadTexture(this);
}

LoadTexture::LoadTexture(std::string new_key, std::string surface_key, AssetsClass::TexturePurpose purpose)
	: key(std::move(new_key))
{
	texture = nullptr;
	from_surface_key = std::move(surface_key);
	texture_purpose = purpose;

	AssetsClass::AddLoadTexture(this);
}

//LoadTexture::LoadTexture(std::string texture_key, TextureCallback load_callback)
//	: key(std::move(texture_key))
//{
//	texture = nullptr;
//	m_LoadCallback = std::move(load_callback);
//
//	AssetsClass::LinkPreloadedTexture(this);
//}

//PregenerateTexture::PregenerateTexture(std::string texture_key, TextureCallback generate_callback)
//	: m_Key(std::move(texture_key)), m_GenerateCallback(std::move(generate_callback))
//{
//	m_Texture = nullptr;
//
//	AssetsClass::LinkPregeneratedTexture(this);
//}

LinkSound::LinkSound(std::string sound_key)
	: m_Key(std::move(sound_key))
{
	m_Sound = nullptr;

	AssetsClass::LinkPreloadedSound(this);
}

Sound *LinkSound::GetSound() const
{
	if (m_Sound == nullptr)
		throw std::runtime_error(FString("[Sound] GetSound '%s' was nullptr", m_Key.c_str()));

	return m_Sound;
}

PreloadFont::PreloadFont(std::string font_key, std::string font_id, float ptsize)
	: m_Key(std::move(font_key)),
	  m_FontID(std::move(font_id))
{
	m_Font = nullptr;
	m_Size = ptsize;

	AssetsClass::PreloadFont_(this);
}

Font *PreloadFont::GetFont() const
{
	if (m_Font == nullptr)
		throw std::runtime_error(FString("[Font] GetFont '%s' was nullptr", m_Key.c_str()));

	return m_Font;
}

TTF_Font *PreloadFont::GetTTFFont() const
{
	bool assets_linked = Assets.GetStatus() == AssetsClass::Status::ASSETS_LINKED;
	assert(assets_linked && "PreloadFont::GetTTFFont() - Assets have not been linked yet");

	bool font_linked = m_Font != nullptr;
	assert(font_linked && FString("PreloadFont::GetTTFFont() - m_Font '%s' was nullptr", m_Key.c_str()).c_str());

	TTF_Font *ttf_font = m_Font->TTFFont();
	bool ttf_font_linked = ttf_font != nullptr;
	assert(ttf_font_linked && FString("PreloadFont::GetTTFFont() - TTF_Font '%s' was nullptr", m_Key.c_str()).c_str());

	return ttf_font;
}

LinkFont::LinkFont(std::string font_key)
	: m_Key(std::move(font_key)),
	  m_FontID("NaN")
{
	m_Font = nullptr;
	m_Size = -1;

	AssetsClass::LinkPreloadedFont(this);
}

Font *LinkFont::GetFont() const
{
	if (m_Font == nullptr)
		throw std::runtime_error(FString("[Font] GetFont '%s' was nullptr", m_Key.c_str()));

	return m_Font;
}
