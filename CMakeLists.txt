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
        src/client/main.cpp
        src/client/core/Clock.cpp
        src/client/core/Clock.h
        deps/icon_resource.rc
        src/shared/utility/Colors.cpp
        src/shared/utility/Colors.h
        src/client/game/entities/characters/character/Character.cpp
        src/client/game/entities/characters/character/Character.h
        src/client/game/entities/Entity.cpp
        src/client/game/entities/Entity.h
        src/client/core/TextManager.cpp
        src/client/core/TextManager.h
        src/client/core/GameControllers.cpp
        src/client/core/GameControllers.h
        src/client/GameData.cpp
        src/client/GameData.h
        src/client/game/GameWorld.cpp
        src/client/game/GameWorld.h
        src/client/game/entities/Projectile.cpp
        src/client/game/entities/Projectile.h
        src/client/core/Drawing.cpp
        src/client/core/Drawing.h
        src/client/game/weapons/projectile/ProjectileWeapon.cpp
        src/client/game/weapons/projectile/ProjectileWeapon.h
        src/client/game/weapons/projectile/WeaponGlock.cpp
        src/client/game/weapons/projectile/WeaponGlock.h
        src/client/game/weapons/projectile/WeaponShotgun.cpp
        src/client/game/weapons/projectile/WeaponShotgun.h
        src/client/game/weapons/projectile/WeaponBurst.cpp
        src/client/game/weapons/projectile/WeaponBurst.h
        src/client/game/weapons/projectile/WeaponMinigun.cpp
        src/client/game/weapons/projectile/WeaponMinigun.h
        src/client/game/indicators/HealthBar.cpp
        src/client/game/indicators/HealthBar.h
        src/client/game/entities/characters/character/Hook.cpp
        src/client/game/entities/characters/character/Hook.h
        src/client/core/AssetsManager.cpp
        src/client/core/AssetsManager.h
        src/client/game/indicators/TextSurface.cpp
        src/client/game/indicators/TextSurface.cpp
        src/client/game/indicators/TextSurface.h
        src/client/game/collision/TileMap.cpp
        src/client/game/collision/TileMap.h
        src/client/game/entities/AmmoBox.cpp
        src/client/game/Player.cpp
        src/client/game/Player.h
        src/client/game/entities/Crate.cpp
        src/client/game/entities/Crate.h
        src/client/game/entities/Error.cpp
        src/client/game/entities/Error.h
        src/client/game/entities/characters/character/Hands.cpp
        src/client/game/entities/characters/character/Hands.h
        src/client/game/entities/characters/CharacterNPC.cpp
        src/client/game/entities/characters/CharacterNPC.h
        src/client/game/entities/item/ItemEntity.cpp
        src/client/game/entities/item/ItemEntity.h
        src/shared/Protocol.h
        src/shared/utility/Vec2.h
        src/shared/utility/Vec2.inl
        src/shared/utility/Randomizer.cpp
        src/shared/utility/Randomizer.h
        src/client/game/entities/item/weapons/EntityGuns.cpp
        src/client/game/entities/item/weapons/EntityGuns.h
        src/client/game/entities/AmmoBox.h
        src/client/game/entities/Crate.cpp
        src/client/game/entities/Crate.h
        src/client/game/entities/Error.cpp
        src/client/game/entities/Error.h
        src/client/game/weapons/projectile/WeaponSniper.cpp
        src/client/game/weapons/projectile/WeaponSniper.h
        src/client/core/Assets.h
        src/client/core/Assets.cpp
        src/client/game/weapons/projectile/WeaponSniper.h
        src/client/game/interface/MainMenu.cpp
        src/client/game/interface/MainMenu.h
        src/client/game/interface/GameModeMenu.cpp
        src/client/game/interface/GameModeMenu.h
        src/client/game/interface/ClassSelectMenu.cpp
        src/client/game/interface/ClassSelectMenu.h
        src/client/game/interface/PauseMenu.cpp
        src/client/game/interface/PauseMenu.h
        src/client/game/interface/LevelUpMenu.cpp
        src/client/game/interface/LevelUpMenu.h
        src/client/game/weapons/projectile/PatersonNavy.cpp
        src/client/game/weapons/projectile/PatersonNavy.h
        src/client/ui/Particles.cpp
        src/client/ui/Particles.h
        src/client/game/error/base/ErrorStatusEffect.cpp
        src/client/game/error/base/ErrorStatusEffect.h
        src/client/game/error/effect/ErrorHealersParadise.h
        src/client/game/error/effect/ErrorHealersParadise.cpp
        src/client/game/error/effect/ErrorDisoriented.h
        src/client/game/error/effect/ErrorDisoriented.cpp
        src/client/game/error/effect/ErrorInvincible.h
        src/client/game/error/effect/ErrorInvincible.cpp
        src/client/game/error/effect/ErrorDangerousRecoil.h
        src/client/game/error/effect/ErrorDangerousRecoil.cpp
        src/client/game/error/effect/ErrorSpiky.h
        src/client/game/error/effect/ErrorSpiky.cpp
        src/client/game/error/effect/ErrorBulletFrenzy.h
        src/client/game/error/effect/ErrorBulletFrenzy.cpp
        src/client/game/error/effect/ErrorConfusingHealth.h
        src/client/game/error/effect/ErrorConfusingHealth.cpp
        src/client/game/error/effect/ErrorTeleport.h
        src/client/game/error/effect/ErrorTeleport.cpp
        src/client/game/error/effect/ErrorSlowdown.h
        src/client/game/error/effect/ErrorSlowdown.cpp
        src/client/game/error/effect/ErrorCantHear.h
        src/client/game/error/effect/ErrorCantHear.cpp
        src/client/game/error/effect/ErrorRangedFists.h
        src/client/game/error/effect/ErrorRangedFists.cpp
        src/client/game/error/ErrorStatuses.cpp
        src/client/game/error/ErrorStatuses.h
        src/client/game/interface/Interface.cpp
        src/client/game/interface/Interface.h
        src/client/game/classes/PlayerClass.cpp
        src/client/game/classes/PlayerClass.h
        src/client/game/classes/classTypes/CyborgClass.cpp
        src/client/game/classes/classTypes/CyborgClass.h
        src/client/game/classes/classTypes/HumanClass.cpp
        src/client/game/classes/classTypes/HumanClass.h
        src/client/game/classes/classTypes/ZombieClass.cpp
        src/client/game/classes/classTypes/ZombieClass.h
        src/client/game/classes/classTypes/VampireClass.cpp
        src/client/game/classes/classTypes/VampireClass.h
        src/client/game/planetaryTrials/PlanetaryCharacter.cpp
        src/client/game/planetaryTrials/PlanetaryCharacter.h
        src/client/game/planetaryTrials/PlanetaryGameWorld.cpp
        src/client/game/planetaryTrials/PlanetaryGameWorld.h
        src/client/game/planetaryTrials/PlanetaryUtils.cpp
        src/client/game/planetaryTrials/PlanetaryUtils.h

)

add_executable(${PROJECT_NAME} ${BUILD_OPTIONS} ${PROJECT_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
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