#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

namespace te::seele {

// ============================================================================
// CONFIGURATION
// ============================================================================

enum class UpscaleMode {
    None,
    Bilinear,
    Bicubic,
    AI_Model,      // Use Seele AI upscaler
    Hybrid,        // AI + traditional sharpening
};

enum class GenerationStyle {
    Organic,       // Natural, flowing forms
    Geometric,     // Angular, structured
    Cyber,         // Digital, glitch-influenced
    Mythic,        // Archetypal, symbolic
    Dystopic,      // Dark, oppressive
};

struct SeeleConfig {
    std::string model_path = "";
    uint32_t upscale_factor = 2;
    float style_strength = 0.5f;
    GenerationStyle generation_style = GenerationStyle::Mythic;
    bool enable_worldbuilding = true;
    bool enable_text_upscale = true;
    bool enable_shader_generation = true;
    uint32_t max_texture_memory_mb = 512;
};

// ============================================================================
// SEELE CORE - Procedural Generation Engine
// ============================================================================

struct ProceduralAsset {
    std::string asset_id;
    std::string type;       // "texture", "model", "shader", "world"
    std::string source_reference;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channels = 4;
    std::vector<uint8_t> data;
    float quality_score = 0.0f;
};

class SeeleAIModule {
public:
    SeeleAIModule() = default;
    ~SeeleAIModule() = default;
    
    bool initialize(const SeeleConfig& config);
    void shutdown();
    
    // Async procedural generation
    void schedule_generation(
        const std::string& asset_id,
        std::function<void(ProceduralAsset)> callback,
        const std::string& seed = ""
    );
    
    // Sync generation (blocking)
    ProceduralAsset generate_texture(
        const std::string& description,
        uint32_t width,
        uint32_t height
    );
    
    std::vector<uint8_t> upscale_image(
        const std::vector<uint8_t>& src_data,
        uint32_t src_w, uint32_t src_h,
        uint32_t target_w, uint32_t target_h
    );
    
    // World building primitives
    struct WorldFeature {
        std::string name;
        std::string type;     // "tree", "building", "character", "NPC"
        std::string archetype;
        float complexity = 1.0f;
        std::vector<std::string> variations;
    };
    
    std::vector<WorldFeature> generate_world_features(
        const std::string& environment_type,
        uint32_t count
    );
    
    // Shader generation
    struct GeneratedShader {
        std::string name;
        std::string vertex_source;
        std::string fragment_source;
        std::vector<std::string> defines;
    };
    
    GeneratedShader generate_world_shader(
        const std::string& lighting_model,
        const std::string& surface_type
    );
    
    // Status queries
    bool is_generating() const { return m_generating; }
    float generation_progress() const { return m_progress; }
    size_t generated_count() const { return m_generated; }
    
private:
    SeeleConfig m_config;
    bool m_initialized = false;
    bool m_generating = false;
    float m_progress = 0.0f;
    size_t m_generated = 0;
    std::string m_model_hash;
};

// ============================================================================
// RENDERER INTEGRATION
// ============================================================================

struct UpscaleParameters {
    UpscaleMode mode = UpscaleMode::AI_Model;
    float denoising_strength = 0.1f;
    bool preserve_colors = true;
    bool enhance_details = true;
    float detail_enhancement = 0.5f;
};

class IUpscaleProcessor {
public:
    virtual ~IUpscaleProcessor() = default;
    virtual bool process_async(
        const uint8_t* src_data,
        uint32_t src_w, uint32_t src_h,
        uint32_t dst_w, uint32_t dst_h,
        std::function<void(const uint8_t*, uint32_t, uint32_t)> result_callback
    ) = 0;
};

// External interface for renderer integration
class SeeleRendererBridge {
public:
    static SeeleRendererBridge& instance();
    
    void set_upscale_params(const UpscaleParameters& params);
    UpscaleParameters get_upscale_params() const { return m_params; }
    
    // Called by renderer before drawing
    void prepare_frame(uint32_t width, uint32_t height);
    
    // Called by renderer for texture upscaling
    bool upscale_texture_upload(uint8_t* data, uint32_t& w, uint32_t& h);
    
private:
    SeeleRendererBridge() = default;
    UpscaleParameters m_params;
    bool m_ready = false;
};

} // namespace te::seele