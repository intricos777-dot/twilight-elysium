#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace te::hackgu {

// ============================================================================
// SHADER GENERATION CONFIGURATION
// ============================================================================

struct ShaderConfig {
    std::string lighting_model = "PBR";      // "PBR", "Toon", "Corruption"
    std::string surface_type = "World";      // "World", "Character", "UI", "Corrupted"
    std::string style = "Cyber";           // "Organic", "Geometric", "Cyber", "Mythic", "Dystopic"
    float corruption_level = 0.0f;           // 0.0 = clean, 1.0 = fully corrupted
    bool enable_data_effects = false;        // Red static, data streams
    bool enable_rain_effect = false;         // For terminal corridors
};

struct MaterialPreset {
    std::string name;
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    std::string albedo_path;
    std::string normal_path;
    std::string emissive_path;
    std::string asset_path;
    float corruption_level = 0.0f;  // Added for corruption tracking
};

// ============================================================================
// WORLD BUILDING DATA STRUCTURES
// ============================================================================

struct WorldChunk {
    std::string chunk_id;
    int x = 0, z = 0;
    uint32_t width = 32;
    uint32_t depth = 32;
    uint32_t height = 32;  // Fixed: was width
    float height_scale = 1.0f;
    float corruption_level = 0.0f;  // Added corruption tracking
    std::vector<MaterialPreset> materials;
    std::vector<std::string> geometry_probes;
};

struct WorldData {
    std::string world_name;
    std::string server_name;
    std::string district;
    std::vector<WorldChunk> chunks;
    std::vector<MaterialPreset> global_materials;
    float corruption_level = 0.0f;
};

// ============================================================================
// SEELE SHADER GENERATOR - Full integration with Twilight Elysium
// ============================================================================

class SeeleShaderGenerator {
public:
    SeeleShaderGenerator() = default;
    ~SeeleShaderGenerator() = default;
    
    // Initialize with AI module
    void initialize();
    
    // Generate PBR shader for world surfaces
    std::string generate_pbr_shader(
        const std::string& vertex_defines = "",
        const std::string& fragment_defines = ""
    );
    
    // Generate corrupted data shader
    std::string generate_corruption_shader(float corruption_level = 0.5f);
    
    // Generate toon shader for characters/UI
    std::string generate_toon_shader();
    
    // Generate particle/material for effects
    std::string generate_particle_shader();
    
    // Generate world-specific shaders
    std::string generate_world_shader(
        const std::string& environment,
        float corruption = 0.0f
    );
    
    // World building
    WorldData generate_world_chunk(
        const std::string& district,
        int chunk_x, int chunk_z,
        float corruption_level = 0.0f
    );
    
    // Generate material from description
    MaterialPreset generate_material(
        const std::string& description,
        const std::string& asset_path = ""
    );
    
    // Apply corruption effect to existing material
    MaterialPreset apply_corruption(
        const MaterialPreset& source,
        float corruption_level = 0.5f
    );
    
private:
    void m_setup_common_defines();
    void m_generate_lighting_functions();
    void m_generate_corruption_functions();
    
    std::string m_vertex_shader_source;
    std::string m_fragment_shader_source;
    std::vector<std::string> m_defines;
};

// ============================================================================
// HASEO RENDERING SYSTEM - Character-specific rendering
// ============================================================================

class HaseoRenderingSystem {
public:
    HaseoRenderingSystem() = default;
    
    // Generate character shader with weapon effects
    std::string generate_character_shader(
        const std::string& character_name,
        bool with_weapon_effect = false
    );
    
    // Generate combo effect shader
    std::string generate_combo_effect();
    
    // Generate data drain effect
    std::string generate_data_drain_effect();
    
    // Generate doppelganger ghost effect
    std::string generate_doppelganger_effect();
};

} // namespace te::hackgu