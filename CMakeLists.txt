cmake_minimum_required(VERSION 3.15)
project(F1Racing VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Raylib configuration
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
set(BUILD_TOOLS    OFF CACHE BOOL "" FORCE)

# Find raylib (adjust path as needed)
find_package(raylib 5.5 QUIET)
if(NOT raylib_FOUND)
    message(WARNING "Raylib not found, using bundled")
    set(RAYLIB_VERSION "5.5")
    set(RAYLIB_URL "https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz")
endif()

# Source files
set(SOURCES
    src/core/Engine.cpp
    src/physics/VehicleBody.cpp
    src/render/Renderer.cpp
    src/render/ShaderManager.cpp
    src/game/Car.cpp
    src/game/Track.cpp
    src/game/RaceManager.cpp
    src/physics/Suspension.cpp
)

set(HEADERS
    src/core/Engine.hpp
    src/core/ECS.hpp
    src/core/EventBus.hpp
    src/physics/VehicleBody.hpp
    src/physics/TireModel.hpp
    src/physics/Suspension.hpp
    src/render/Renderer.hpp
    src/render/ShaderManager.hpp
    src/render/Camera.hpp
    src/game/Car.hpp
    src/game/Track.hpp
    src/game/RaceManager.hpp
    src/main.cpp
)

# Create executable
add_executable(f1_racing ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(f1_racing PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link libraries
if(raylib_FOUND)
    target_link_libraries(f1_racing raylib)
else()
    # If using system raylib
    target_link_libraries(f1_racing 
        raylib 
        gdi32 
        winmm 
        kernel32
    )
endif()

# Platform-specific libraries (Windows)
if(WIN32)
    target_link_libraries(f1_racing 
        gdi32 
        winmm 
        kernel32
    )
endif()

# Copy raylib.dll to output directory (Windows)
if(WIN32)
    add_custom_command(TARGET f1_racing POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:raylib>
            $<TARGET_FILE_DIR:f1_racing>
    )
endif()

