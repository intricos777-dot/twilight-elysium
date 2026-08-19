#pragma once
#include "model_generator.h"
#include "seele_world_renderer.h"
#include <vector>
#include <string>

namespace te {

// ============================================================================
// COMPLETE WORLD BUILDER - Assembles all 3D world data for any disc
// ============================================================================

// All 10 discs + 4 GU volumes
enum class DiscID {
    SIGN, INFECTION, FREQUENCY, OUTBREAK, QUARANTINE,
    LINK, QUANTUM, FIND_ME,
    GU_VOL1, GU_VOL2, GU_VOL3, GU_VOL4,
    TOTAL
};

struct DiscInfo {
    std::string id;
    std::string title;
    std::string era;       // "R1", "R2", "Link"
    std::string arc;       // "infection", "gu", "sign", "link"
    int canon_order;
    bool is_anime;
    std::vector<std::string> zone_names;
    std::vector<std::string> key_events;
    std::string protagonist;
    std::string theme_file;
};

// World theme (CSS colors, fonts, UI)
struct WorldTheme {
    std::string disc_id;
    std::string primary_color;
    std::string secondary_color;
    std::string accent_color;
    std::string bg_color;
    std::string text_color;
    std::string border_color;
    std::string font_family;
    std::string font_size;
    std::string font_weight;
    float scanline_intensity;
    float glow_intensity;
    float fog_density;
    std::string sky_shader;
    std::string ground_shader;
};

class CompleteWorldBuilder {
public:
    CompleteWorldBuilder();
    ~CompleteWorldBuilder();

    // Initialize with renderer and AI
    bool initialize(SDL2Renderer* renderer, seele::SeeleAIModule* ai);
    void shutdown();

    // Load disc data from JSON
    bool load_disc_definitions(const std::string& json_path);
    bool load_theme(const std::string& json_path);
    
    // Build a complete disc (zones, models, textures, lighting)
    bool build_disc(const std::string& disc_id);
    
    // Build a specific zone within a disc
    bool build_zone(const std::string& disc_id, const std::string& zone_name);
    
    // Build all zones for all discs
    void build_all();
    
    // Switch active disc/zone
    void set_active_disc(const std::string& disc_id);
    void set_active_zone(const std::string& zone_name);
    
    // Render the active world
    void render_active(float dt);
    
    // Access
    SeeleWorldRenderer* world_renderer() { return m_world_renderer; }
    ModelGenerator* model_generator() { return m_model_gen; }
    
    // Stats
    int total_models() const { return m_model_gen ? m_model_gen->generated_count() : 0; }
    int total_zones() const { return (int)m_zones.size(); }
    int total_objects() const { return m_world_renderer ? m_world_renderer->object_count() : 0; }
    
    // Get disc info
    DiscInfo get_disc_info(const std::string& disc_id) const;
    std::vector<std::string> get_all_disc_ids() const;
    std::vector<std::string> get_zone_names(const std::string& disc_id) const;

private:
    SDL2Renderer* m_renderer = nullptr;
    seele::SeeleAIModule* m_ai = nullptr;
    SeeleWorldRenderer* m_world_renderer = nullptr;
    ModelGenerator* m_model_gen = nullptr;
    
    // Disc data
    std::map<std::string, DiscInfo> m_discs;
    std::map<std::string, WorldTheme> m_themes;
    
    // Zone data
    struct ZoneData {
        std::string id;
        std::string name;
        std::string type;
        int sky[3];
        int ground[3];
        int fog;
        std::string mood;
        std::string shader;
        std::string texture_set;
        std::string music_track;
        std::vector<std::string> structures;
        std::vector<std::string> npcs;
        std::vector<std::string> monsters;
        std::vector<std::string> items;
        std::vector<std::string> avatars;
    };
    std::map<std::string, std::vector<ZoneData>> m_zones;  // disc_id -> zones
    
    // Active state
    std::string m_active_disc;
    std::string m_active_zone;
    bool m_built = false;
    
    // Parse helpers
    bool parse_disc_definitions(const std::string& json);
    bool parse_theme(const std::string& json);
    bool parse_zones(const std::string& json, const std::string& disc_id);
    
    // Apply theme to world renderer
    void apply_theme(const WorldTheme& theme);
    
    // Generate disc-specific content
    void generate_disc_structures(const std::string& disc_id, ZoneData& zone);
    void generate_disc_npcs(const std::string& disc_id, ZoneData& zone);
    void generate_disc_monsters(const std::string& disc_id, ZoneData& zone);
    void generate_disc_items(const std::string& disc_id, ZoneData& zone);
};

// ============================================================================
// GAME ENGINE INTEGRATION
// ============================================================================

// Integrates with the existing .hack game engine
class World3DIntegration {
public:
    World3DIntegration();
    ~World3DIntegration();

    bool initialize(SDL2Renderer* renderer, seele::SeeleAIModule* ai);
    void shutdown();

    // Load and display a disc
    bool load_disc(const std::string& disc_id);
    
    // Run the 3D world viewer
    void run();
    
    // Process events
    void handle_event(const void* event);
    
    // Render frame
    void render_frame(float dt);

private:
    SDL2Renderer* m_renderer = nullptr;
    seele::SeeleAIModule* m_ai = nullptr;
    CompleteWorldBuilder* m_builder = nullptr;
    bool m_running = false;
};

} // namespace te
