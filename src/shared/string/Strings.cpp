//
// Created by Matq on 19/12/2024.
//

#include "shared/string/Strings.h"
#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdarg>
#include <algorithm>
#include <random>

// ANSI color codes
const std::unordered_map<char, std::string> minecraftToAnsi = {
    { '0', "\033[30m" }, // Black
    { '1', "\033[34m" }, // Dark Blue
    { '2', "\033[32m" }, // Dark Green
    { '3', "\033[36m" }, // Dark Aqua
    { '4', "\033[31m" }, // Dark Red
    { '5', "\033[35m" }, // Dark Purple
    { '6', "\033[33m" }, // Gold
    { '7', "\033[37m" }, // Gray
    { '8', "\033[90m" }, // Dark Gray
    { '9', "\033[94m" }, // Blue
    { 'a', "\033[92m" }, // Green
    { 'b', "\033[96m" }, // Aqua
    { 'c', "\033[91m" }, // Red
    { 'd', "\033[95m" }, // Light Purple
    { 'e', "\033[93m" }, // Yellow
    { 'f', "\033[97m" }, // White
    { 'r', "\033[0m" }   // Reset
};

namespace Strings {
std::string FStringColors(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Determine the required buffer size
    va_list argsCopy;
    va_copy(argsCopy, args);
    int bufferSize = std::vsnprintf(nullptr, 0, format, argsCopy) + 1; // +1 for null terminator
    va_end(argsCopy);

    // Create the buffer with the required size
    char* buffer = new char[bufferSize];

    // Format the string
    std::vsnprintf(buffer, bufferSize, format, args);
    va_end(args);

    std::string message(buffer);
    delete[] buffer;

    // Replace Minecraft-style color codes with ANSI codes
    size_t pos = 0;
    while ((pos = message.find('&', pos)) != std::string::npos) {
        if (pos + 1 < message.length()) {
            char colorCode = message[pos + 1];
            auto it = minecraftToAnsi.find(colorCode);
            if (it != minecraftToAnsi.end()) {
                // Replace &<code> with the corresponding ANSI code
                message.replace(pos, 2, it->second);
                continue; // Skip past the replacement
            }
        }
        ++pos; // Move past this '&'
    }

    return message + "\033[0m"; // Ensure the string ends with a reset code
}

std::string FString(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Determine the required buffer size
    va_list argsCopy;
    va_copy(argsCopy, args);
    int bufferSize = std::vsnprintf(nullptr, 0, format, argsCopy) + 1;  // +1 for null terminator
    va_end(argsCopy);

    // Create the buffer with the required size
    char* buffer = new char[bufferSize];

    // Format the string
    std::vsnprintf(buffer, bufferSize, format, args);
    va_end(args);
    std::string message(buffer);
    delete[] buffer;
    return message;
}

std::string ErasePrefix(std::string string, const std::string& prefix) {
    size_t pos = string.find(prefix);
    if (pos != std::string::npos)
        string.erase(pos, prefix.length());

    return string;
}

std::string EraseSuffix(std::string string, const std::string& suffix) {
    size_t pos = string.rfind(suffix);
    if (pos != std::string::npos && pos + suffix.length() == string.length())
        string.erase(pos, suffix.length());

    return string;
}

std::wstring RepeatStringW(const std::wstring& str, int times) {
    std::wstring result;
    for (int i = 0; i < times; ++i)
        result += str;

    return result;
}

std::string RepeatString(const std::string& str, int times) {
    std::string result;
    for (int i = 0; i < times; ++i)
        result += str;

    return result;
}

std::string ToUpperCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::toupper(c);
    });
    return result;
}

std::string ToLowerCase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

//char RandomUppercaseLetter() {
//    return 'A' + (std::rand() % 26);
//}

//std::string GenerateRandomFilename(size_t length, const std::string& extension) {
//	static const char characters[] =
//		"0123456789"
//		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//		"abcdefghijklmnopqrstuvwxyz";
//
//	std::random_device rd;
//	std::mt19937 generator(rd());
//	std::uniform_int_distribution<> distribution(0, sizeof(characters) - 2);
//
//	std::string filename;
//	for (size_t i = 0; i < length; ++i) {
//		filename += characters[distribution(generator)];
//	}
//
//	return filename + extension;
//}

std::string ConvertTimeNano(long long nanoseconds) {
    const auto day = static_cast<long long>(86400e9L);
    const auto hour = static_cast<long long>(3600e9L);
    const auto minute = static_cast<long long>(60e9L);
    const auto second = static_cast<long long>(1e9L);
    const auto millisecond = static_cast<long long>(1e6L);
    const auto microsecond = static_cast<long long>(1e3L);

    if (nanoseconds > day) return FString("%lldd", nanoseconds / day);
    else if (nanoseconds > hour) return FString("%lldh", nanoseconds / hour);
    else if (nanoseconds > minute) return FString("%lldmin", nanoseconds / minute);
    else if (nanoseconds > second) return FString("%llds", nanoseconds / second);
    else if (nanoseconds > millisecond) return FString("%lldms", nanoseconds / millisecond);
    else if (nanoseconds > microsecond) return FString("%lldus", nanoseconds / microsecond);
    else return FString("%lldns", nanoseconds);
}

// Cout
void PrintDivider(const std::string& label) {
    const int LENGTH = 72;

    std::string output;
    auto label_length = label.size();
    if (label_length == 0) {
        output = Strings::FStringColors("&8%s", Strings::RepeatString("-", LENGTH).c_str());
    } else {
        auto new_length = LENGTH - label_length;
        auto half = new_length / 2;
        auto remaining = new_length - half;
        output = Strings::FStringColors("&8%s", Strings::RepeatString("-", (int)half).c_str());
        output += Strings::FStringColors("&f%s", label.c_str());
        output += Strings::FStringColors("&8%s", Strings::RepeatString("-", (int)remaining).c_str());
    }

	std::cout << output << "\n";
}

}