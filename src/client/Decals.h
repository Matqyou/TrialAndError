//
// Created by Mat on 12/3/2024.
//

#pragma once

#include <iostream>
#include <memory> // For std::unique_ptr
#include <mutex>  // For std::call_once and std::once_flag
#include <vector>

class Decals {
    static Decals* Instance;

public:
    static void initialize();
    static void deinitialize();
    static Decals* Get();

    Decals(const Decals&) = delete;
    Decals& operator=(const Decals&) = delete;

private:
    Decals();
    ~Decals();

};
