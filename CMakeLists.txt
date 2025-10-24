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
    src/client/core/Assets.cpp
    src/client/core/Assets.h
    src/client/core/AssetsManager.cpp
    src/client/core/AssetsManager.h
    src/client/core/Clock.cpp
    src/client/core/Clock.h
    src/client/core/Drawing.cpp
    src/client/core/Drawing.h
    src/client/core/GameControllers.cpp
    src/client/core/GameControllers.h
    src/client/core/TextManager.cpp
    src/client/core/TextManager.h
    src/client/game/collision/TileMap.cpp
    src/client/game/collision/TileMap.h
    src/client/game/entities/cartesian/AmmoBox.cpp
    src/client/game/entities/cartesian/AmmoBox.h
    src/client/game/entities/cartesian/characters/character/Character.cpp
    src/client/game/entities/cartesian/characters/character/Character.h
    src/client/game/entities/cartesian/characters/character/Hands.cpp
    src/client/game/entities/cartesian/characters/character/Hands.h
    src/client/game/entities/cartesian/characters/character/Hook.cpp
    src/client/game/entities/cartesian/characters/character/Hook.h
    src/client/game/entities/cartesian/characters/CharacterNPC.cpp
    src/client/game/entities/cartesian/characters/CharacterNPC.h
    src/client/game/entities/cartesian/Crate.cpp
    src/client/game/entities/cartesian/Crate.h
    src/client/game/entities/cartesian/Entity.cpp
    src/client/game/entities/cartesian/Entity.h
    src/client/game/entities/cartesian/Error.cpp
    src/client/game/entities/cartesian/Error.h
    src/client/game/entities/cartesian/Planet.cpp
    src/client/game/entities/cartesian/Planet.h
    src/client/game/entities/cartesian/Projectile.cpp
    src/client/game/entities/cartesian/Projectile.h
    # Non-cartesian duplicate Character files removed to avoid duplicate symbol definitions
    src/client/game/entities/item/ItemEntity.cpp
    src/client/game/entities/item/ItemEntity.h
    src/client/game/entities/item/weapons/EntityGuns.cpp
    src/client/game/entities/item/weapons/EntityGuns.h
    src/client/game/entities/planetary/characters/character/PlanetaryCharacter.cpp
    src/client/game/entities/planetary/characters/character/PlanetaryCharacter.h
    src/client/game/entities/planetary/characters/PlanetaryCharacterNPC.cpp
    src/client/game/entities/planetary/characters/PlanetaryCharacterNPC.h
    src/client/game/entities/planetary/PlanetaryEntity.cpp
    src/client/game/entities/planetary/PlanetaryEntity.h
    src/client/game/error/base/ErrorStatusEffect.cpp
    src/client/game/error/base/ErrorStatusEffect.h
    src/client/game/error/effect/ErrorBulletFrenzy.cpp
    src/client/game/error/effect/ErrorBulletFrenzy.h
    src/client/game/error/effect/ErrorCantHear.cpp
    src/client/game/error/effect/ErrorCantHear.h
    src/client/game/error/effect/ErrorConfusingHealth.cpp
    src/client/game/error/effect/ErrorConfusingHealth.h
    src/client/game/error/effect/ErrorDangerousRecoil.cpp
    src/client/game/error/effect/ErrorDangerousRecoil.h
    src/client/game/error/effect/ErrorDisoriented.cpp
    src/client/game/error/effect/ErrorDisoriented.h
    src/client/game/error/effect/ErrorHealersParadise.cpp
    src/client/game/error/effect/ErrorHealersParadise.h
    src/client/game/error/effect/ErrorInvincible.cpp
    src/client/game/error/effect/ErrorInvincible.h
    src/client/game/error/effect/ErrorRangedFists.cpp
    src/client/game/error/effect/ErrorRangedFists.h
    src/client/game/error/effect/ErrorSlowdown.cpp
    src/client/game/error/effect/ErrorSlowdown.h
    src/client/game/error/effect/ErrorSpiky.cpp
    src/client/game/error/effect/ErrorSpiky.h
    src/client/game/error/effect/ErrorTeleport.cpp
    src/client/game/error/effect/ErrorTeleport.h
    src/client/game/error/ErrorStatuses.cpp
    src/client/game/error/ErrorStatuses.h
    src/client/game/indicators/HealthBar.cpp
    src/client/game/indicators/HealthBar.h
    src/client/game/indicators/TextSurface.cpp
    src/client/game/indicators/TextSurface.h
    src/client/game/interface/ClassSelectMenu.cpp
    src/client/game/interface/ClassSelectMenu.h
    src/client/game/interface/GameModeMenu.cpp
    src/client/game/interface/GameModeMenu.h
    src/client/game/interface/Interface.cpp
    src/client/game/interface/Interface.h
    src/client/game/interface/LevelUpMenu.cpp
    src/client/game/interface/LevelUpMenu.h
    src/client/game/interface/MainMenu.cpp
    src/client/game/interface/MainMenu.h
    src/client/game/interface/PauseMenu.cpp
    src/client/game/interface/PauseMenu.h
    src/client/game/interface/PlanetEnterMenu.cpp
    src/client/game/interface/PlanetEnterMenu.h
    src/client/game/players/classes/ClassTypes/CyborgClass.cpp
    src/client/game/players/classes/ClassTypes/CyborgClass.h
    src/client/game/players/classes/ClassTypes/HumanClass.cpp
    src/client/game/players/classes/ClassTypes/HumanClass.h
    src/client/game/players/classes/ClassTypes/VampireClass.cpp
    src/client/game/players/classes/ClassTypes/VampireClass.h
    src/client/game/players/classes/ClassTypes/ZombieClass.cpp
    src/client/game/players/classes/ClassTypes/ZombieClass.h
    src/client/game/players/classes/PlayerClass.cpp
    src/client/game/players/classes/PlayerClass.h
    src/client/game/players/Player.cpp
    src/client/game/players/Player.h
    src/client/game/weapons/projectile/PatersonNavy.cpp
    src/client/game/weapons/projectile/PatersonNavy.h
    src/client/game/weapons/projectile/ProjectileWeapon.cpp
    src/client/game/weapons/projectile/ProjectileWeapon.h
    src/client/game/weapons/projectile/WeaponBurst.cpp
    src/client/game/weapons/projectile/WeaponBurst.h
    src/client/game/weapons/projectile/WeaponGlock.cpp
    src/client/game/weapons/projectile/WeaponGlock.h
    src/client/game/weapons/projectile/WeaponMinigun.cpp
    src/client/game/weapons/projectile/WeaponMinigun.h
    src/client/game/weapons/projectile/WeaponShotgun.cpp
    src/client/game/weapons/projectile/WeaponShotgun.h
    src/client/game/weapons/projectile/WeaponSniper.cpp
    src/client/game/weapons/projectile/WeaponSniper.h
    src/client/game/world/cartesian/CartesianGameWorld.cpp
    src/client/game/world/cartesian/CartesianGameWorld.h
    src/client/game/world/GameWorld.cpp
    src/client/game/world/GameWorld.h
    src/client/game/world/planetary/PlanetaryGameWorld.cpp
    src/client/game/world/planetary/PlanetaryGameWorld.h
    src/client/game/world/planetary/PlanetaryUtils.cpp
    src/client/game/world/planetary/PlanetaryUtils.h
    src/client/GameData.cpp
    src/client/GameData.h
    src/client/main.cpp
    src/client/ui/Particles.cpp
    src/client/ui/Particles.h
    src/shared/Protocol.h
    src/shared/utility/Colors.cpp
    src/shared/utility/Colors.h
    src/shared/utility/Randomizer.cpp
    src/shared/utility/Randomizer.h
    src/shared/utility/Vec2.h
    src/shared/utility/Vec2.inl
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

if (WIN32)
    # Location of the built exe
    set(EXE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.exe")

    # Path to the Desktop
    file(TO_CMAKE_PATH "$ENV{USERPROFILE}/Desktop" DESKTOP_PATH)

    # Shortcut destination (.lnk file)
    set(SHORTCUT_PATH "${DESKTOP_PATH}/${PROJECT_NAME}.lnk")

    # Post-build shortcut creation disabled to avoid PowerShell quoting/execution issues
    # add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    #     COMMENT "Creating desktop shortcut..."
    #     COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command "try { $Wsh = New-Object -ComObject WScript.Shell; $Shortcut = $Wsh.CreateShortcut('${SHORTCUT_PATH}'); $Shortcut.TargetPath = '${EXE_PATH}'; $Shortcut.WorkingDirectory = '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'; $Shortcut.IconLocation = '${EXE_PATH},0'; $Shortcut.Save(); Write-Host 'Shortcut created at: ${SHORTCUT_PATH}'; } catch { Write-Warning 'Could not create shortcut (no permission?)'; }" )
endif()