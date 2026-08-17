#pragma once

#include "ai/seele_ai.h"
#include "game/legend_of_dragoon/world/world.h"
#include <functional>
#include <string>
#include <vector>
#include <map>

namespace te::hackgu {

// ============================================================================
// .hack//G.U. SPECIFIC CONTENT GENERATION
// ============================================================================

struct DataItem {
    std::string id;
    std::string name;
    std::string description;
    std::string type;  // "weapon", "armor", "item", "key"
    int level_requirement = 1;
    std::vector<std::string> effects;
};

struct CharacterProfile {
    std::string name;
    std::string player_name;
    std::string race;  // "Haseo", "BlackRose", "Kite", "Balmung", etc.
    std::string guild;
    int level = 1;
    int hp = 100;
    int mp = 50;
    std::vector<DataItem> equipment;
};

struct DungeonStage {
    std::string id;
    std::string name;
    std::string environment;  // "Macroservers", "Delta Force", etc.
    int difficulty = 1;
    std::vector<std::string> enemies;
    CharacterProfile* npcs = nullptr;
};

struct AssetData {
    std::string asset_id;
    std::string name;
    std::string type;
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint8_t> pixel_data;
    float resolution_scale = 1.0f;
};

class HackguContentGenerator {
public:
    HackguContentGenerator(seele::SeeleAIModule* ai);
    ~HackguContentGenerator() = default;
    
    // Generate Infection arc content
    void generate_infection_content();
    
    // Generate GU Returner arc content  
    void generate_gu_content();
    
    // Generate world features for a specific server/district
    std::vector<seele::SeeleAIModule::WorldFeature> 
        generate_district_features(const std::string& district);
    
    // Generate corrupted data textures
    seele::ProceduralAsset generate_corrupted_texture(
        const std::string& base_asset,
        float corruption_level = 0.5f
    );
    
    // Generate player model variations
    std::vector<AssetData> generate_character_models(
        const CharacterProfile& profile
    );
    
    // Generate quest content
    struct Quest {
        std::string id;
        std::string title;
        std::string description;
        std::vector<std::string> objectives;
        std::vector<DataItem> rewards;
    };
    
    Quest generate_quest(
        const std::string& arc,  // "infection" or "gu"
        int quest_number
    );
    
    // Generate UI themes
    seele::ProceduralAsset generate_ui_theme(
        const std::string& theme_name,
        uint32_t width = 1024,
        uint32_t height = 1024
    );
    
private:
    seele::SeeleAIModule* m_ai = nullptr;
    std::vector<DataItem> m_data_items;
    std::vector<CharacterProfile> m_characters;
    std::vector<DungeonStage> m_stages;
    
    void generate_character_palettes();
    void generate_ui_elements();
};

} // namespace te::hackgu