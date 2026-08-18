#include "renderer/sdl2_renderer.h"
#include "renderer/seele_world_renderer.h"
#include "ai/seele_ai.h"
#include "engine/engine.h"
#include <cstdio>
#include <cstring>

using namespace te;

int main(int argc, char** argv) {
    bool headless = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--headless") == 0) headless = true;
    }
    
    std::printf("=== Twilight Elysium - Seele AI World Renderer ===\n");
    
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "Twilight Elysium - Seele AI World",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        return 1;
    }
    
    // Initialize renderer
    SDL2Renderer renderer;
    if (!renderer.initialize(window)) {
        std::fprintf(stderr, "Failed to initialize renderer\n");
        return 1;
    }
    
    // Initialize Seele AI
    seele::SeeleConfig ai_cfg;
    ai_cfg.enable_worldbuilding = true;
    ai_cfg.enable_shader_generation = true;
    ai_cfg.generation_style = seele::GenerationStyle::Cyber;
    
    seele::SeeleAIModule ai;
    ai.initialize(ai_cfg);
    
    // Initialize world renderer
    SeeleWorldRenderer world_renderer;
    if (!world_renderer.initialize(&renderer, &ai)) {
        std::fprintf(stderr, "Failed to initialize world renderer\n");
        return 1;
    }
    
    // Load original game assets
    world_renderer.load_original_assets("/home/sin/workspace/dot-hack-remake/Content/Assets/seele");
    
    // Load zone data from Seele-generated worlds
    world_renderer.load_zone_data(
        "/home/sin/workspace/dot-hack-remake/Content/Worlds/seele_worlds.json",
        "vol1_i"
    );
    
    // Generate the 3D world
    world_renderer.generate_world("vol1_i", "mac_anu");
    
    std::printf("[World] objects=%d meshes=%d textures=%d\n",
        world_renderer.object_count(),
        world_renderer.mesh_count(),
        world_renderer.texture_count());
    
    // Main loop
    bool running = true;
    SDL_Event event;
    uint32_t frame = 0;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (event.type == SDL_MOUSEMOTION) {
                world_renderer.rotate_camera(event.motion.xrel * 0.5f, event.motion.yrel * 0.5f);
            }
            if (event.type == SDL_MOUSEWHEEL) {
                world_renderer.zoom_camera(event.wheel.y * 0.1f);
            }
        }
        
        // Render
        renderer.begin_frame();
        world_renderer.render(1.0f / 60.0f);
        renderer.end_frame();
        
        frame++;
        if (headless && frame >= 60) break;  // Headless: render 60 frames then exit
    }
    
    std::printf("[World] rendered %d frames\n", frame);
    
    // Cleanup
    world_renderer.shutdown();
    ai.shutdown();
    renderer.shutdown();
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    std::printf("=== Done ===\n");
    return 0;
}
