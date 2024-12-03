//
// Created by Mat on 12/3/2024.
//

#include "Decals.h"
#include <filesystem>
#include <fstream>
#include <unordered_set>
#include <algorithm>

Decals* Decals::Instance = nullptr;

Decals::Decals()
{
    std::cout << "Decals constructor" << std::endl;
}

Decals::~Decals()
{
    std::cout << "Decals constructor" << std::endl;
}

void Decals::initialize()
{
    if (Instance == nullptr)
    {
        std::cout << "Decals has been initialized." << std::endl;
        Instance = new Decals();

        // Todo: Loading assets in realtime (adding/removing files?)
        // Load assets (decals specifically)
        const char* ASSETS_DIRECTORY = "./assets/images";
        std::unordered_set<std::string> supportedExtensions = {
            ".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tif", ".tiff", ".webp"
        };

        for (const auto& entry : std::filesystem::recursive_directory_iterator(ASSETS_DIRECTORY)) {
            if (entry.is_regular_file()) {
                std::filesystem::path filePath = entry.path();
                std::string extension = filePath.extension().string(); // Get the file extension

                // Convert extension to lowercase for case-insensitive comparison
                for (auto& ch : extension) {
                    ch = std::tolower(ch);
                }

                // Check if the extension is supported
                if (supportedExtensions.find(extension) != supportedExtensions.end()) {
                    // Remove the "assets" and "images" part from the path
                    std::string pathStr = filePath.string();

                    // Remove the "./assets/images" part (if present)
                    std::string removeStr = "./assets/images";
                    size_t pos = pathStr.find(removeStr);
                    if (pos != std::string::npos) {
                        pathStr.erase(pos, removeStr.length());
                    }

                    // Remove the leading separator
                    if (pathStr[0] == '/' || pathStr[0] == '\\') {
                        pathStr.erase(0, 1);
                    }

                    // Convert to lowercase
                    std::transform(pathStr.begin(), pathStr.end(), pathStr.begin(), ::tolower);

                    // Replace directory separators with period "."
                    std::replace(pathStr.begin(), pathStr.end(), '/', '.');
                    std::replace(pathStr.begin(), pathStr.end(), '\\', '.');

                    // pathStr
                } else {
                    std::cout << "Unsupported decals in images: " << filePath.string() << std::endl;
                }
            }
        }
    }
}

void Decals::deinitialize()
{
    std::cout << "Decals has been deinitialized." << std::endl;
    delete Instance;
    Instance = nullptr;
}


Decals* Decals::Get()
{
    if (Instance == nullptr) {
        throw std::runtime_error("Decals not initialized. Call initialize() first.");
    }

    return Instance;
}
