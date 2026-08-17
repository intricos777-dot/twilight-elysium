#include "hackgu_content.h"
#include <thread>
#include <random>
#include <sstream>
#include <algorithm>

namespace te::hackgu {

HackguContentGenerator::HackguContentGenerator(seele::SeeleAIModule* ai) 
    : m_ai(ai) {}

void HackguContentGenerator::generate_infection_content() {
    // Generate Infection arc content (DISC 1-4)
    
    // Character: Haseo
    CharacterProfile haseo;
    haseo.name = "Haseo";
    haseo.player_name = "Haseo";
    haseo.race = "Player";
    haseo.guild = "Haseo's Guild";
    haseo.level = 1;
    haseo.hp = 100;
    haseo.mp = 50;
    
    // Data: Ryuken weapon
    DataItem ryuken;
    ryuken.id = "weapon_001";
    ryuken.name = "Ryuken";
    ryuken.description = "A short sword that can strike multiple times.";
    ryuken.type = "weapon";
    ryuken.level_requirement = 1;
    ryuken.effects = {"Multi-hit damage"};
    m_data_items.push_back(ryuken);
    
    haseo.equipment.push_back(ryuken);
    m_characters.push_back(haseo);
    
    // Generate stages for Infection
    std::vector<std::string> infection_stages = {
        "moon_tree", "lost_cemetery", "sanctuary", "end_trail"
    };
    
    for (const auto& stage_id : infection_stages) {
        DungeonStage stage;
        stage.id = stage_id;
        stage.difficulty = 1;
        stage.environment = "Macroservers";
        m_stages.push_back(stage);
    }
    
    // Generate corrupted UI theme
    generate_ui_theme("infection_theme", 512, 512);
}

void HackguContentGenerator::generate_gu_content() {
    // Generate GU Returner arc content (Vol 1-4)
    
    // Character: Asuka
    CharacterProfile asuka;
    asuka.name = "Asuka";
    asuka.player_name = "Asuka";
    asuka.race = "Player";
    asuka.guild = "Moonstone";
    asuka.level = 15;
    asuka.hp = 450;
    asuka.mp = 200;
    m_characters.push_back(asuka);
    
    // Generate stages for GU
    std::vector<std::string> gu_stages = {
        "vol1_mac_anu", "vol1_delta_gate", "vol2_octa_case", 
        "vol3_data_realm", "vol4_quarantine"
    };
    
    for (const auto& stage_id : gu_stages) {
        DungeonStage stage;
        stage.id = stage_id;
        stage.difficulty = static_cast<int>(stage_id[3] - '0');
        stage.environment = "Delta Server";
        m_stages.push_back(stage);
    }
    
    // Generate Returner arc UI theme
    generate_ui_theme("gu_theme", 512, 512);
}

std::vector<seele::SeeleAIModule::WorldFeature> 
HackguContentGenerator::generate_district_features(const std::string& district) {
    
    // Map districts to appropriate environment types
    std::string environment;
    if (district == "mac_anu" || district == "central") {
        environment = "urban";
    } else if (district == "delta") {
        environment = "dungeons";
    } else if (district == "data_realm") {
        environment = "cyber";
    } else {
        environment = "default";
    }
    
    // Generate features
    auto features = m_ai->generate_world_features(environment, 50);
    
    // Annotate with .hack-specific types
    for (auto& feature : features) {
        feature.name = district + "_" + feature.type + "_" + feature.name;
    }
    
    return features;
}

seele::ProceduralAsset HackguContentGenerator::generate_corrupted_texture(
    const std::string& base_asset,
    float corruption_level
) {
    // Generate corrupted version of an asset
    seele::ProceduralAsset asset = m_ai->generate_texture(
        "corrupted " + base_asset + " with static noise and red distortion",
        512, 512
    );
    
    // Apply corruption effect
    std::mt19937 rng(std::hash<std::string>{}(base_asset));
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    float corruption = std::clamp(corruption_level, 0.0f, 1.0f);
    
    for (size_t i = 0; i < asset.data.size(); i += 4) {
        // Red channel increase for corruption
        int r = asset.data[i];
        int g = asset.data[i + 1];
        int b = asset.data[i + 2];
        
        r = static_cast<int>(r * (1.0f + corruption));
        b = static_cast<int>(b * (1.0f - corruption * 0.5f));
        
        if (dist(rng) < corruption * 0.3f) {
            // Add static pixels
            r = g = b = 255;
        }
        
        asset.data[i] = static_cast<uint8_t>(std::clamp(r, 0, 255));
        asset.data[i + 1] = static_cast<uint8_t>(std::clamp(g, 0, 255));
        asset.data[i + 2] = static_cast<uint8_t>(std::clamp(b, 0, 255));
    }
    
    return asset;
}

std::vector<AssetData> HackguContentGenerator::generate_character_models(
    const CharacterProfile& profile
) {
    std::vector<AssetData> models;
    
    // Generate character renderings
    std::vector<std::string> styles = {"front", "side", "back"};
    
    for (const auto& style : styles) {
        AssetData model;
        model.asset_id = profile.player_name + "_" + style;
        model.name = profile.name;
        model.type = "character_" + style;
        model.width = 256;
        model.height = 512;
        
        // Generate based on race/type
        std::string desc = profile.name + " " + style + " view, .hack character style";
        
        if (m_ai) {
            auto tex = m_ai->generate_texture(desc, model.width, model.height);
            model.pixel_data = tex.data;
            model.resolution_scale = 2.0f;
        }
        
        models.push_back(model);
    }
    
    return models;
}

HackguContentGenerator::Quest HackguContentGenerator::generate_quest(
    const std::string& arc,
    int quest_number
) {
    Quest quest;
    quest.id = arc + "_q" + std::to_string(quest_number);
    quest.title = arc + " Quest " + std::to_string(quest_number);
    
    // Generate quest description based on arc
    if (arc == "infection") {
        quest.description = "A strange data anomaly has been detected in a nearby dungeon. Clear it out.";
        quest.objectives = {
            "Enter dungeon",
            "Defeat enemies",
            "Locate anomaly",
            "Return to terminal"
        };
        DataItem reward;
        reward.id = "key_001";
        reward.name = "Data Key";
        reward.description = "Opens restricted area";
        reward.type = "key";
        quest.rewards.push_back(reward);
    } else {
        quest.description = "The Returner's data has been fragmented. Collect the pieces.";
        quest.objectives = {
            "Search data realm",
            "Collect data fragments",
            "Defeat corrupted player",
            "Report findings"
        };
        
        DataItem exp_reward;
        exp_reward.id = "exp_100";
        exp_reward.name = "EXP +100";
        exp_reward.description = "Increases level";
        exp_reward.type = "exp";
        quest.rewards.push_back(exp_reward);
        
        DataItem item_reward;
        item_reward.id = "item_001";
        item_reward.name = "Healing Chip";
        item_reward.description = "Restores HP";
        item_reward.type = "item";
        quest.rewards.push_back(item_reward);
    }
    
    return quest;
}

seele::ProceduralAsset HackguContentGenerator::generate_ui_theme(
    const std::string& theme_name,
    uint32_t width,
    uint32_t height
) {
    std::string desc;
    if (theme_name.find("infection") != std::string::npos) {
        desc = ".hack // infection UI theme, blood red accents, digital overlay";
    } else {
        desc = ".hack // GU Returner theme, blue/cyan cyber aesthetic";
    }
    
    return m_ai->generate_texture(desc, width, height);
}

void HackguContentGenerator::generate_character_palettes() {
    // Generate color palettes for .hack characters
    // This would be used by shaders for character rendering
}

void HackguContentGenerator::generate_ui_elements() {
    // Generate UI elements: buttons, panels, icons
}

} // namespace te::hackgu