#include "engine.h"
#include "../renderer/renderer.h"
#include "../ai/canon_story.h"
#include "../game/hackgu/hackgu_world_builder.h"
#include <cstdio>
#include <cstring>

namespace {

int bounded_smoke() {
    te::Engine& engine = te::Engine::instance();
    uint64_t ticks = 0;
    uint64_t last_frame = 0;
    engine.attach_tick([&](float, uint64_t frame) { ++ticks; last_frame = frame; });

    te::EngineConfig cfg;
    cfg.window_title = "Twilight Elysium (headless)";
    cfg.window_width = 320;
    cfg.window_height = 180;
    cfg.scale_mode = 0;
    if (!engine.initialize(cfg)) {
        std::fprintf(stderr, "[smoke] engine init failed\n");
        return 1;
    }
    engine.run(60);

    const bool ticked = ticks == 60u && last_frame == 60u;
    std::printf("[smoke] ran %llu frames, tick fired %llu times - %s\n",
                (unsigned long long)last_frame, (unsigned long long)ticks,
                ticked ? "PASS" : "FAIL");
    engine.shutdown();
    return ticked ? 0 : 1;
}

int hackgu_discs_smoke() {
    using namespace te::hackgu;
    DiscWorldBuilder builder;
    builder.register_disc({
        "disc1",
        ".hack//G.U. Disc 1: Infection",
        "infection",
        1,
        "/home/sin/Projects/hackgu-modding/extract/vol1_i/vol1/data/data",
        {}
    });
    builder.register_disc({
        "disc2",
        ".hack//G.U. Disc 2: Rebirth",
        "gu_returner",
        2,
        "/home/sin/Projects/hackgu-modding/extract/vol2_a/vol2/data/data",
        {}
    });
    builder.register_disc({
        "disc3",
        ".hack//G.U. Disc 3: Reminisce",
        "gu_returner",
        3,
        "/home/sin/Projects/hackgu-modding/extract/vol3_a/vol3/data/data",
        {}
    });
    builder.register_disc({
        "disc4",
        ".hack//G.U. Disc 4: Quarantine",
        "gu_returner",
        4,
        "/home/sin/Projects/hackgu-modding/extract/vol2_a_pc/vol2/data/data",
        {}
    });

    const auto discs = builder.available_discs();
    bool ok = discs.size() == 4;
    std::printf("[hackgu] registered discs: %zu\n", discs.size());
    for (const auto& d : discs) {
        std::printf("[hackgu]   - %s: %s\n", d.id.c_str(), d.title.c_str());
        if (!d.title.starts_with(".hack")) ok = false;
    }

    bool built_all = true;
    for (const auto& d : discs) {
        if (!builder.build_world_for_disc(d.id)) {
            std::fprintf(stderr, "[hackgu] build failed for %s\n", d.id.c_str());
            built_all = false;
        }
    }

    const bool passed = ok && built_all;
    std::printf("[hackgu] discs smoke - %s\n", passed ? "PASS" : "FAIL");
    return passed ? 0 : 1;
}

} // namespace

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--bounded") == 0) return bounded_smoke();
        if (std::strcmp(argv[i], "--hackgu-discs") == 0) return hackgu_discs_smoke();
    }

    te::Engine engine;
    te::EngineConfig cfg;
    cfg.window_title = "Twilight Elysium";
    cfg.window_width = 1280;
    cfg.window_height = 720;
    cfg.base_width = 640;
    cfg.base_height = 360;
    cfg.scale_mode = 4;

    if (argc > 1 && std::strcmp(argv[1], "--auto") == 0) cfg.scale_mode = 5;
    else if (argc > 1 && std::strcmp(argv[1], "--integer") == 0) { cfg.scale_mode = 0; cfg.base_width = 1920; cfg.base_height = 1080; }
    else if (argc > 1 && std::strcmp(argv[1], "--fractional") == 0) cfg.scale_mode = 1;
    else if (argc > 1 && std::strcmp(argv[1], "--fsr") == 0) cfg.scale_mode = 3;
    else if (argc > 1 && std::strcmp(argv[1], "--optimized") == 0) cfg.scale_mode = 4;

    if (!engine.initialize(cfg)) {
        std::fprintf(stderr, "Failed to initialize engine\n");
        return 1;
    }

    std::printf("[main] engine initialized\n");
    engine.run();
    engine.shutdown();
    return 0;
}