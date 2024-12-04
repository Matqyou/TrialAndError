cmake_minimum_required(VERSION 3.23)
project(TrialAndError)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(deps/SDL2/include deps/SDL2_image/include deps/SDL2_mixer/include deps/SDL2_ttf/include)
link_directories(deps/SDL2/lib/x64 deps/SDL2_image/lib/x64 deps/SDL2_mixer/lib/x64 deps/SDL2_ttf/lib/x64)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# if (CMAKE_BUILD_TYPE STREQUAL "Release")
#     set(BUILD_OPTIONS WIN32)
# endif ()
# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(BUILD_OPTIONS)
# endif ()

set(PROJECT_FILES
        "src/main.cpp"
        "src/technical stuff/Clock.cpp"
        "src/technical stuff/Clock.h"
        "deps/icon_resource.rc"
        "src/technical stuff/Colors.cpp"
        "src/technical stuff/Colors.h"
        "src/game/entities/characters/character/Character.cpp"
        "src/game/entities/characters/character/Character.h"
        "src/game/entities/Entity.cpp"
        "src/game/entities/Entity.h"
        "src/technical stuff/TextManager.cpp"
        "src/technical stuff/TextManager.h"
        "src/technical stuff/GameControllers.cpp"
        "src/technical stuff/GameControllers.h"
        "src/GameReference.cpp"
        "src/GameReference.h"
        "src/game/GameWorld.cpp"
        "src/game/GameWorld.h"
        "src/game/entities/Projectile.cpp"
        "src/game/entities/Projectile.h"
        "src/technical stuff/ImageManager.cpp"
        "src/technical stuff/ImageManager.h"
        "src/technical stuff/Drawing.cpp"
        "src/technical stuff/Drawing.h"
        "src/technical stuff/SoundManager.cpp"
        "src/technical stuff/SoundManager.h"
        "src/game/weapons/projectile/ProjectileWeapon.cpp"
        "src/game/weapons/projectile/ProjectileWeapon.h"
        "src/game/weapons/projectile/WeaponGlock.cpp"
        "src/game/weapons/projectile/WeaponGlock.h"
        "src/game/weapons/projectile/WeaponShotgun.cpp"
        "src/game/weapons/projectile/WeaponShotgun.h"
        "src/game/weapons/projectile/WeaponBurst.cpp"
        "src/game/weapons/projectile/WeaponBurst.h"
        "src/game/weapons/projectile/WeaponMinigun.cpp"
        "src/game/weapons/projectile/WeaponMinigun.h"
        "src/game/indicators/HealthBar.cpp"
        "src/game/indicators/HealthBar.h"
        "src/game/entities/characters/character/Hook.cpp"
        "src/game/entities/characters/character/Hook.h"
        "src/technical stuff/AssetsManager.cpp"
        "src/technical stuff/AssetsManager.h"
        "src/game/indicators/TextSurface.cpp"
        "src/game/indicators/TextSurface.cpp"
        "src/game/indicators/TextSurface.h"
        "src/game/collision/TileMap.cpp"
        "src/game/collision/TileMap.h"
        "src/game/entities/AmmoBox.cpp"
        "src/game/Player.cpp"
        "src/game/Player.h"
        "src/game/entities/Crate.cpp"
        "src/game/entities/Crate.h"
        "src/game/entities/Error.cpp"
        "src/game/entities/Error.h"
        "src/game/entities/characters/character/Hands.cpp"
        "src/game/entities/characters/character/Hands.h"
        "src/game/entities/characters/CharacterNPC.cpp"
        "src/game/entities/characters/CharacterNPC.h"
        "src/game/weapons/laser/LaserWeapon.cpp"
        "src/game/weapons/laser/LaserWeapon.h"
        "src/game/entities/item/ItemEntity.cpp"
        "src/game/entities/item/ItemEntity.h"
        "src/Protocol.h"
        "src/technical stuff/Vec2.h"
        "src/technical stuff/Vec2.inl"
        "src/technical stuff/Randomizer.cpp"
        "src/technical stuff/Randomizer.h"
        "src/game/entities/item/weapons/EntityGuns.cpp"
        "src/game/entities/item/weapons/EntityGuns.h"
        "src/game/entities/AmmoBox.h"
        "src/game/entities/Crate.cpp"
        "src/game/entities/Crate.h"
        "src/game/entities/Error.cpp"
        "src/game/entities/Error.h"
        "src/game/weapons/projectile/WeaponSniper.cpp"
        "src/game/weapons/projectile/WeaponSniper.h")

add_executable(${PROJECT_NAME} ${BUILD_OPTIONS} ${PROJECT_FILES})

target_link_libraries(${PROJECT_NAME} SDL2 SDL2main SDL2_image SDL2_mixer SDL2_ttf)
target_link_options(${PROJECT_NAME} PRIVATE "-static")

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

function(copy_files SOURCE_DIR DEST_DIR)
    file(GLOB items "${SOURCE_DIR}/*")
    foreach (item ${items})
        if (IS_DIRECTORY ${item})
            get_filename_component(dirname ${item} NAME)
            file(MAKE_DIRECTORY ${DEST_DIR}/${dirname})
            copy_files(${item} ${DEST_DIR}/${dirname})
        else ()
            get_filename_component(filename ${item} NAME)
            configure_file(${item} ${DEST_DIR}/${filename} COPYONLY)
        endif ()
    endforeach ()
endfunction()

copy_files(${CMAKE_CURRENT_SOURCE_DIR}/bin ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
# Add automatic shortcut pls