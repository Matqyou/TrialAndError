project(TrialAndError)
cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(deps/SDL2/include) # C:/deps/SDL2_image/include
link_directories(deps/SDL2/lib/x64) # C:/deps/SDL2_image/lib/x64

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(BUILD_OPTIONS WIN32)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_OPTIONS)
endif()

add_executable(${PROJECT_NAME} ${BUILD_OPTIONS} src/main.cpp src/Clock.cpp src/Clock.h deps/icon_resource.rc src/Colors.cpp src/Colors.h src/entities/Character.cpp src/entities/Character.h)
target_link_libraries(${PROJECT_NAME} SDL2main SDL2) # SDL2_image

function(copy_files SOURCE_DIR DEST_DIR)
    file(GLOB files "${SOURCE_DIR}/*")
    foreach(file ${files})
        get_filename_component(filename ${file} NAME)
        configure_file(${file} ${DEST_DIR}/${filename} COPYONLY)
    endforeach()
endfunction()

copy_files(${CMAKE_CURRENT_SOURCE_DIR}/bin ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
# Add automatic shortcut pls
