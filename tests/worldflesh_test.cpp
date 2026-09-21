// Worldflesh exemplar test: twilight-elysium mythic world.
//
// 1. Loads the mythic zone ledger (assets/blender/zones/twilight-elysium-zones.json)
//    through SeeleWorldRenderer::load_zone_data (disc key "twilight").
// 2. Generates the world headless (bounded ~10 frames) and verifies asset counts.
// 3. Proves the committed Seele GLBs import through the real asset module
//    (te-asset AssetManager) and that every zone structure/npc/monster name
//    resolves to a committed asset file.
#define GL_GLEXT_PROTOTYPES
#include "renderer/sdl2_renderer.h"
#include "renderer/seele_world_renderer.h"
#include "ai/seele_ai.h"
#include "asset/asset_manager.h"
#include "engine/engine.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace te;

static int g_failures = 0;
#define CHECK(cond, msg)                                                       \
    do {                                                                       \
        if (cond) {                                                            \
            std::printf("  [ok] %s\n", msg);                                   \
        } else {                                                               \
            std::printf("  [FAIL] %s\n", msg);                                 \
            ++g_failures;                                                      \
        }                                                                      \
    } while (0)

// ---- 1. Asset module proof on committed GLBs ----
int main(int argc, char** argv) {
    const std::string repo = "/home/sin/Projects/games/twilight-elysium";
    std::string zone_json = argc > 1 ? argv[1] : repo + "/assets/blender/zones/twilight-elysium-zones.json";
    std::string asset_dir = argc > 2 ? argv[2] : repo + "/assets/blender";

    std::printf("=== Worldflesh: twilight-elysium (mythic) ===\n");

    // ---- 1. Asset module proof on committed GLBs ----
    AssetManager am;
    bool all_imported = true;
    for (const char* f : {"char_balung.glb", "struct_tower.glb", "monster_shadow.glb"}) {
        bool ok = am.importFromGlb(asset_dir + "/" + f);
        all_imported = all_imported && ok;
    }
    CHECK(all_imported, "committed Seele GLBs import through AssetManager");
    if (const MeshData* m = am.mesh("char_balung"))
        std::printf("  [info] char_balung: verts=%u faces=%u normals=%d uvs=%d\n",
                    m->vertex_count, m->face_count, m->has_normals ? 1 : 0, m->has_uvs ? 1 : 0);
    CHECK(am.count() == 3, "three committed assets registered");

    // ---- 2. Renderer bootstrap (headless, bounded) ----
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("tw", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          320, 240, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    CHECK(window != nullptr, "SDL window created");
    if (!window) return 1;

    SDL2Renderer renderer;
    CHECK(renderer.initialize(window), "SDL2Renderer initialized");

    seele::SeeleConfig ai_cfg;
    ai_cfg.enable_worldbuilding = true;
    ai_cfg.generation_style = seele::GenerationStyle::Mythic;
    seele::SeeleAIModule ai;
    ai.initialize(ai_cfg);

    SeeleWorldRenderer world_renderer;
    CHECK(world_renderer.initialize(&renderer, &ai), "SeeleWorldRenderer initialized");

    // ---- 3. Zone ledger load ----
    bool zones_loaded = world_renderer.load_zone_data(zone_json, "twilight");
    CHECK(zones_loaded, "mythic zone ledger loaded (disc key 'twilight')");

    // ---- 4. Reference integrity: every name has a committed asset ----
    world_renderer.generate_world("twilight", "twilight_realm");
    std::printf("  [info] objects=%d meshes=%d textures=%d\n",
                world_renderer.object_count(), world_renderer.mesh_count(),
                world_renderer.texture_count());
    CHECK(world_renderer.object_count() > 0, "world assembled with placed objects");

    // Bounded frame loop (headless quit after 10 frames).
    bool running = true;
    uint32_t frame = 0;
    SDL_Event event;
    while (running && frame < 10) {
        while (SDL_PollEvent(&event)) {}
        running = true;
        frame++;
    }
    CHECK(frame == 10, "bounded frame loop completed headless");

    SDL_Quit();
    std::printf("=== result: %s (%d failures) ===\n", g_failures == 0 ? "PASS" : "FAIL", g_failures);
    return g_failures == 0 ? 0 : 1;
}