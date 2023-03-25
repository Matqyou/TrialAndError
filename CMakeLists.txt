cmake_minimum_required(VERSION 3.23)
project(TrialAndError)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(deps/SDL2/include deps/SDL2_image/include deps/SDL2_mixer/include deps/SDL2_ttf/include)
link_directories(deps/SDL2/lib/x64 deps/SDL2_image/lib/x64 deps/SDL2_mixer/lib/x64 deps/SDL2_ttf/lib/x64)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

set(PROJECT_FILES
        "src/main.cpp"
        "src/technical stuff/Clock.cpp"
        "src/technical stuff/Clock.h"
        "deps/icon_resource.rc"
        "src/technical stuff/Colors.cpp"
        "src/technical stuff/Colors.h"
        "src/entities/Character.cpp"
        "src/entities/Character.h"
        "src/entities/Entity.cpp"
        "src/entities/Entity.h"
        "src/technical stuff/TextManager.cpp"
        "src/technical stuff/TextManager.h"
        "src/technical stuff/GameControllers.cpp"
        "src/technical stuff/GameControllers.h"
        "src/GameReference.cpp"
        "src/GameReference.h"
        "src/GameWorld.cpp"
        "src/GameWorld.h"
        "src/entities/Bullets.cpp"
        "src/entities/Bullets.h"
        "src/technical stuff/ImageManager.cpp"
        "src/technical stuff/ImageManager.h")
add_executable(${PROJECT_NAME} ${PROJECT_FILES})
target_link_libraries(${PROJECT_NAME} SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(WIN32)
        target_compile_options(${PROJECT_NAME} PRIVATE "-mconsole")
    elseif(APPLE)
        target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-export-dynamic")
    else()
        # No special configuration needed for console output on Linux
    endif()
else()
    if(WIN32)
        set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    elseif(APPLE)
        set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

function(copy_files SOURCE_DIR DEST_DIR)
    file(GLOB items "${SOURCE_DIR}/*")
    foreach(item ${items})
        if(IS_DIRECTORY ${item})
            get_filename_component(dirname ${item} NAME)
            file(MAKE_DIRECTORY ${DEST_DIR}/${dirname})
            copy_files(${item} ${DEST_DIR}/${dirname})
        else()
            get_filename_component(filename ${item} NAME)
            configure_file(${item} ${DEST_DIR}/${filename} COPYONLY)
        endif()
    endforeach()
endfunction()

copy_files(${CMAKE_CURRENT_SOURCE_DIR}/bin ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
# Add automatic shortcut pls
