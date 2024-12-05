//
// Created by Mat on 12/3/2024.
//

#include "Decals.h"
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include <algorithm>
#include <cstring>
#include <utility>
#include "../technical stuff/TextManager.h"

Decals* Decals::Instance = nullptr;

Texture2::Texture2(std::string key, SDL_Texture* sdl_texture, std::string load_extension)
: m_Key(std::move(key)) {
    m_SDLTexture = sdl_texture;
    m_LoadExtension = std::move(load_extension);

    m_Information = {};
    SDL_QueryTexture(m_SDLTexture,
                     &m_Information.format,
                     &m_Information.access,
                     &m_Information.w,
                     &m_Information.h);
}

Texture2::~Texture2() {
    if (m_SDLTexture)
        SDL_DestroyTexture(m_SDLTexture);
}

void Texture2::SetBlendMode(SDL_BlendMode blend_mode) {
    SDL_SetTextureBlendMode(m_SDLTexture, blend_mode);
}

void Texture2::SetColorMod(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(m_SDLTexture, r, g, b);
}

void Texture2::SetAlphaMod(int alpha) {
    SDL_SetTextureAlphaMod(m_SDLTexture, alpha);
}

Decals::Decals(SDL_Renderer* renderer): m_InvalidTexture(nullptr)
{
    // Todo: Loading assets in realtime (adding/removing files?)
    // Load assets (decals specifically)
    const char* ASSETS_DIRECTORY = R"(.\assets\images\)";
    std::unordered_set<std::string> supportedExtensions = {
        ".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tif", ".tiff", ".webp"
    };

    for (const auto& entry : std::filesystem::recursive_directory_iterator(ASSETS_DIRECTORY)) {
        if (!entry.is_regular_file())
            continue;

        const std::filesystem::path& file_path = entry.path();

        // Extension in lowercase
        const std::string extension = file_path.extension().string();
        std::string extension_lower(extension);
        std::transform(extension_lower.begin(), extension_lower.end(), extension_lower.begin(), ::tolower);

        // Check if the extension is supported
        if (supportedExtensions.find(extension) == supportedExtensions.end()) {
            std::cout << "Unsupported texture format: " << file_path.string() << std::endl;
            continue;
        }

        // Process the path string for logging or other use
        std::string identificator = file_path.string();
        identificator = ErasePrefix(identificator, ASSETS_DIRECTORY);
        identificator = EraseSuffix(identificator, extension);

        // Convert to lowercase and replace slashes to periods
        std::transform(identificator.begin(), identificator.end(), identificator.begin(), ::tolower);
        std::replace(identificator.begin(), identificator.end(), '/', '.');
        std::replace(identificator.begin(), identificator.end(), '\\', '.');

        auto it = m_Textures.find(identificator);
        if (it != m_Textures.end()) {
            std::cout << FString("Attempted to load duplicate texture '%s' (%s)", identificator.c_str(), it->second->m_LoadExtension.c_str()) << std::endl;
            continue;
        }

        // Load the texture
        SDL_Surface* TempSurface = IMG_Load(file_path.string().c_str());
        if (!TempSurface) {
            std::cout << FString("Failed to load texture '%s'", file_path.string().c_str()) << std::endl;
            continue;
        }

        SDL_Texture* NewSDLTexture = SDL_CreateTextureFromSurface(renderer, TempSurface);
        SDL_FreeSurface(TempSurface);

        // Add it to all the textures
        auto new_texture = new Texture2(identificator, NewSDLTexture, extension_lower);
        m_Textures[identificator] = new_texture;
    }

    std::cout << FString("Loaded %i textures", m_Textures.size()) << std::endl;
    m_InvalidTexture = nullptr;
    m_InvalidTexture = GetTexture("invalid");
}

Decals::~Decals()
{
    size_t num_destroyed = 0;
    for (const auto& entry : m_Textures) {
        delete entry.second;
        num_destroyed++;
    }
    std::cout << FString("Unloaded %zu textures", num_destroyed) << std::endl;
}

void Decals::initialize(SDL_Renderer* renderer)
{
    if (Instance == nullptr)
        Instance = new Decals(renderer);
}

void Decals::deinitialize()
{
    delete Instance;
    Instance = nullptr;
}

Decals* Decals::Get()
{
    if (Instance == nullptr)
        throw std::runtime_error("Decals not initialized. Call initialize() first.");

    return Instance;
}

Texture2* Decals::GetTexture(const std::string& texture_key) {
    auto it = m_Textures.find(texture_key);
    if (it != m_Textures.end())
        return it->second;

    std::cout << FString("Texture '%s' not found", texture_key.c_str()) << std::endl;
    return m_InvalidTexture;
}
