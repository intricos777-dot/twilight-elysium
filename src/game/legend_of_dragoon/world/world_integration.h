#pragma once

#include "ai/seele_ai.h"
#include "game/legend_of_dragoon/world/world.h"
#include <functional>
#include <string>

namespace te::lod {

// ============================================================================
// SEELE-AUGMENTED WORLD GENERATION
// ============================================================================

class SeeleWorldGenerator {
public:
    SeeleWorldGenerator() = default;
    ~SeeleWorldGenerator() = default;
    
    void set_ai_module(seele::SeeleAIModule* ai) { m_ai = ai; }
    
    // Generate a procedurally-created stage
    void generate_stage_async(
        const std::string& stage_description,
        std::function<void(StageData)> on_complete
    );
    
    // Generate stage with specific parameters
    StageData generate_stage(
        const std::string& id,
        const std::string& environment = "default",
        uint32_t width_tiles = 32,
        uint32_t height_tiles = 32
    );
    
    // Upscale existing world assets
    void upscale_world_textures(float upscale_factor = 2.0f);
    
    // Generate shaders for the world
    void generate_world_shaders();
    
    // Get generated features
    std::vector<seele::SeeleAIModule::WorldFeature> get_features() const { return m_features; }
    
private:
    seele::SeeleAIModule* m_ai = nullptr;
    std::vector<seele::SeeleAIModule::WorldFeature> m_features;
    seele::SeeleConfig m_config;
};

} // namespace te::lod