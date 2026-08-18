cmake_minimum_required(VERSION 3.20)
project(twilight-elysium VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(te-engine STATIC
    src/engine/engine.cpp
    src/engine/memory.cpp
    src/engine/threading.h
    src/engine/input.cpp
    src/engine/resolution_scaler.cpp
    src/engine/math_types.cpp
    src/engine/entity.cpp
)
target_include_directories(te-engine PUBLIC src)

add_library(te-renderer STATIC
    src/renderer/renderer.cpp
    src/renderer/sdl2_renderer.cpp
    src/renderer/seele_world_renderer.cpp
    src/renderer/model_generator.cpp
    src/renderer/complete_world_builder.cpp
    src/renderer/material/material.cpp
)
target_include_directories(te-renderer PUBLIC src)
target_link_libraries(te-renderer PRIVATE te-engine ${CMAKE_DL_LIBS})

# Find SDL2 and GL
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
find_package(OpenGL REQUIRED)

target_include_directories(te-renderer PUBLIC src ${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
target_link_libraries(te-renderer PUBLIC ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})

add_library(te-shader STATIC
    src/shader/shader_stub.cpp
)
target_link_libraries(te-shader PRIVATE te-engine)

add_library(te-asset STATIC
    src/asset/asset_stub.cpp
)
target_link_libraries(te-asset PRIVATE te-engine)

add_library(te-pipeline STATIC
    src/pipeline/frame_graph_stub.cpp
)
target_link_libraries(te-pipeline PRIVATE te-engine)

# Seele AI Module - procedural generation, upscaling, and canon story
add_library(te-ai STATIC
    src/ai/seele_ai.cpp
    src/ai/hackgu_content.cpp
    src/ai/canon_story.cpp
)
target_include_directories(te-ai PUBLIC src)

# Seele Shader Generation Module - 3D world building
add_library(te-shading STATIC
    src/game/legend_of_dragoon/rendering/seele_shader.cpp
)

# .hack//G.U. remake module built on Twilight Elysium
add_library(te-hackgu STATIC
    src/game/hackgu/hackgu_world_builder.cpp
    src/ai/hackgu_content.cpp
)
target_link_libraries(te-hackgu PRIVATE te-ai te-lod)
target_include_directories(te-hackgu PUBLIC src)

add_library(te-lod STATIC
    src/game/legend_of_dragoon/character/character.cpp
    src/game/legend_of_dragoon/combat/combat.cpp
    src/game/legend_of_dragoon/world/world.cpp
    src/game/legend_of_dragoon/world/world_integration.cpp
    src/game/legend_of_dragoon/stage/stage.cpp
    src/game/legend_of_dragoon/legend_of_dragoon.cpp
    src/game/legend_of_dragoon/progression/progression.cpp
    src/game/legend_of_dragoon/menu/menu.cpp
    src/platform/local_runtime.cpp
    src/platform/satalight_runtime.cpp
    src/platform/runtime_factory.cpp
    src/platform/distribution/satalight_distributor.cpp
)
target_link_libraries(te-lod PRIVATE 
    te-engine 
    te-renderer 
    te-shader 
    te-pipeline 
    te-ai
    te-shading
)

# Test executables
enable_testing()
add_executable(te-test src/engine/main.cpp)
target_link_libraries(te-test PRIVATE te-engine te-lod te-hackgu te-renderer)

# SDL2 world renderer test
add_executable(te-sdl2-world tests/sdl2_world_test.cpp)
target_link_libraries(te-sdl2-world PRIVATE te-engine te-renderer te-ai te-hackgu)

# Complete world builder test
add_executable(te-complete tests/complete_world_test.cpp)
target_link_libraries(te-complete PRIVATE te-engine te-renderer te-ai te-hackgu)

# Headless bounded smoke: the engine loop must close and tick 60 frames
add_test(NAME te-core COMMAND te-test --bounded)
add_test(NAME te-hackgu-discs COMMAND te-test --hackgu-discs)
add_test(NAME te-sdl2-world COMMAND te-sdl2-world --headless)