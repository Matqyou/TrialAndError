//
// Created by Matq on 28/03/2025.
//

#include "HitboxFile.h"
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

HitboxFile::HitboxFile() {
    this->hitbox = Rect4i(0, 0, 0, 0);
    this->uses_hitbox = false;
    this->loaded = false;
}

HitboxFile::HitboxFile(const Rect4i& hitbox, bool uses_hitbox) {
    this->hitbox = hitbox;
    this->uses_hitbox = uses_hitbox;
    this->loaded = true;
}

HitboxFile::~HitboxFile() {

}

void HitboxFile::Clear() {
    loaded = false;
}

bool HitboxFile::SaveFile(const std::string& directory, const std::string& entry_name) const {
    // Convert dots to the correct directory separator
    std::string formatted_entry = entry_name;
    std::replace(formatted_entry.begin(), formatted_entry.end(), '.', static_cast<char>(std::filesystem::path::preferred_separator));

    // Construct the full file path
    std::filesystem::path file_path = std::filesystem::path(directory) / formatted_entry;
    file_path += ".vis";

    if (!loaded) {
        std::cout << "Can't save undefined data to: " << file_path.string() << std::endl;
        return false;
    }

    // If `loaded` is false, delete the file
    if (!uses_hitbox) {
        if (std::filesystem::exists(file_path)) {
            if (std::filesystem::remove(file_path)) {
                std::cout << "Deleted file: " << file_path.string() << std::endl;
            } else { std::cerr << "Failed to delete file: " << file_path.string() << std::endl; }
        } else { std::cerr << "File does not exist: " << file_path.string() << std::endl; }
        return true; // Exit function after deletion
    }

    // Ensure the directories exist
    std::filesystem::create_directories(file_path.parent_path());

    // Open the file for writing
    std::ofstream file(file_path);
    if (!file) {
        std::cerr << "Error opening file for writing: " << file_path << std::endl;
        return false;
    }

    // Write hitbox numbers to the file
    file << hitbox.x << " " << hitbox.y << " " << hitbox.w << " " << hitbox.h << " ";

    file.close();
    std::cout << "Saved numbers to: " << file_path.string() << std::endl;
    return true;
}

bool HitboxFile::OpenFile(const std::string& directory, const std::string& entry_name) {
    std::vector<int> numbers;

    // Convert dots to the correct directory separator
    std::string formatted_entry = entry_name;
    std::replace(formatted_entry.begin(), formatted_entry.end(), '.', static_cast<char>(std::filesystem::path::preferred_separator));

    // Construct the full file path
    std::filesystem::path file_path = std::filesystem::path(directory) / formatted_entry;
    file_path += ".vis";

    // Open the file for reading
    std::ifstream file(file_path);
    if (!file) {
        loaded = false;
        return false;
    }

    // Read exactly 4 numbers from the file
    int num;
    while (file >> num) {
        numbers.push_back(num);
        if (numbers.size() > 4) break; // Prevent reading extra numbers
    }

    file.close();

    // Validate that exactly 4 numbers were read
    if (numbers.size() != 4) {
        std::cerr << "Error: Expected 4 numbers, but found " << numbers.size() << " numbers in " << file_path << std::endl;
        loaded = false;
        return false;
    }

    // Assign values to hitbox
    hitbox = Rect4i(numbers[0], numbers[1], numbers[2], numbers[3]);
    uses_hitbox = true;
    loaded = true;

    std::cout << "Successfully loaded hitbox: "
              << numbers[0] << " " << numbers[1] << " "
              << numbers[2] << " " << numbers[3] << " from "
              << file_path.string() << std::endl;
    return true;
}

void HitboxFile::SetHitbox(const Rect4i& hitbox, bool uses_hitbox) {
    this->hitbox = hitbox;
    this->uses_hitbox = uses_hitbox;
    this->loaded = true;
}
