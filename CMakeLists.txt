cmake_minimum_required(VERSION 3.23)
project(TrialAndError)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# MinGW specific fixes
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-keep-inline-dllexport")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-keep-inline-dllexport")
    # Add additional flags for template issues
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")
endif()

# Configure SDL3 libraries before adding subdirectories
set(SDL3IMAGE_DEPS_SHARED OFF CACHE BOOL "Use shared libraries for SDL3_image dependencies" FORCE)
set(SDL3IMAGE_VENDORED ON CACHE BOOL "Use vendored libraries for SDL3_image" FORCE)

set(SDL3MIXER_DEPS_SHARED OFF CACHE BOOL "Use shared libraries for SDL3_mixer dependencies" FORCE)
set(SDL3MIXER_VENDORED ON CACHE BOOL "Use vendored libraries for SDL3_mixer" FORCE)

# SDL3_ttf specific options - try multiple variable name variations
set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored libraries for SDL3_ttf" FORCE)

# Try all possible variations of HarfBuzz disable flags
set(SDL3TTF_HARFBUZZ OFF CACHE BOOL "Enable harfbuzz support" FORCE)
set(SDL_TTF_HARFBUZZ OFF CACHE BOOL "Enable harfbuzz support" FORCE)
set(SDLTTF_HARFBUZZ OFF CACHE BOOL "Enable harfbuzz support" FORCE)
set(TTF_USE_HARFBUZZ OFF CACHE BOOL "Enable harfbuzz support" FORCE)

# Try all possible variations of SVG disable flags
set(SDL3TTF_SVG OFF CACHE BOOL "Enable SVG support" FORCE)
set(SDL_TTF_SVG OFF CACHE BOOL "Enable SVG support" FORCE)
set(SDLTTF_SVG OFF CACHE BOOL "Enable SVG support" FORCE)
#set(SDLTTF_PLUTOSVG OFF CACHE BOOL "Enable SVG support" FORCE)

# Disable external dependencies completely
set(SDL3TTF_DEPS_SHARED OFF CACHE BOOL "Use shared libraries for dependencies" FORCE)
set(SDL3TTF_INSTALL OFF CACHE BOOL "Install SDL3_ttf" FORCE)

# Additional HarfBuzz disable flags
set(HB_HAVE_FREETYPE OFF CACHE BOOL "Disable harfbuzz freetype" FORCE)
set(HB_BUILD_SUBSET OFF CACHE BOOL "Disable harfbuzz subset" FORCE)
set(HB_BUILD_UTILS OFF CACHE BOOL "Disable harfbuzz utils" FORCE)
set(HB_BUILD_TESTS OFF CACHE BOOL "Disable harfbuzz tests" FORCE)

# Try to disable all optional features in FreeType as well
set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "Disable harfbuzz in freetype" FORCE)

# Add SDL3 submodules
add_subdirectory(external/SDL3)
add_subdirectory(external/SDL3_image)
add_subdirectory(external/SDL3_mixer)
add_subdirectory(external/SDL3_ttf)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

file(GLOB_RECURSE PROJECT_FILES "src/*.cpp" "src/*.h" "src/*.inl" "res/icon_resource.rc")

add_executable(${PROJECT_NAME} ${PROJECT_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE src)

# Link SDL3 libraries from submodules
target_link_libraries(${PROJECT_NAME}
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_mixer::SDL3_mixer
        SDL3_ttf::SDL3_ttf
)

# For static linking - try without this first, then add back if needed
# target_link_options(${PROJECT_NAME} PRIVATE "-static")

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

# Copy SDL3 DLLs to executable directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SDL3.dll"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_image::SDL3_image>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SDL3_image.dll"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_mixer::SDL3_mixer>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SDL3_mixer.dll"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3_ttf::SDL3_ttf>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying SDL3_ttf.dll"
)


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