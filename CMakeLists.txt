cmake_minimum_required(VERSION 2.8.4)
project(moonrat)

add_subdirectory(vendor/include/assimp ../vendor/include/assimp/build)
include_directories(vendor/include/assimp/build)
include_directories(vendor/include/assimp/include)
include_directories(vendor/include/assimp/code)

add_subdirectory(vendor/lib/freetype2 ../vendor/lib/freetype2/build)
include_directories(vendor/lib/freetype2/build)
include_directories(vendor/lib/freetype2/include)

add_executable(
        launcher

        src/main.cpp

        src/memory/Allocator.hpp
        src/memory/Provider.cpp
        src/memory/Provider.hpp
        src/memory/Pool.cpp
        src/memory/Pool.hpp

        src/events/Input.cpp
        src/events/Input.hpp

        src/renderer/Shader.cpp
        src/renderer/Shader.hpp
        src/renderer/Window.hpp
        src/renderer/Window.cpp
        src/renderer/Text.cpp
        src/renderer/Text.hpp
        src/renderer/BulkText.cpp
        src/renderer/BulkText.hpp
        src/renderer/Camera.cpp
        src/renderer/Camera.hpp
        src/renderer/Object3D.cpp
        src/renderer/Object3D.hpp
        src/renderer/BulkObject3D.cpp
        src/renderer/BulkObject3D.hpp
        src/renderer/Player.cpp
        src/renderer/Player.hpp
        src/renderer/NPC.cpp
        src/renderer/NPC.hpp
        src/renderer/Interactions.cpp
        src/renderer/Interactions.hpp
)

target_link_libraries(launcher SDL2 SDL2_image GL GLEW freetype assimp)