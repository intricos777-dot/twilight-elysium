#define GL_GLEXT_PROTOTYPES
#include "complete_world_builder.h"
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace te {

// ============================================================================
// JSON PARSER HELPERS
// ============================================================================

static std::string extractJsonString(const std::string& s, size_t& pos) {
    while (pos < s.size() && s[pos] != '"') pos++;
    if (pos >= s.size()) return "";
    pos++;
    size_t start = pos;
    while (pos < s.size() && s[pos] != '"') {
        if (s[pos] == '\\') pos++;
        pos++;
    }
    std::string result = s.substr(start, pos - start);
    if (pos < s.size()) pos++;
    return result;
}

static int extractJsonInt(const std::string& s, size_t& pos) {
    while (pos < s.size() && (s[pos] < '0' || s[pos] > '9') && s[pos] != '-') pos++;
    int sign = 1;
    if (pos < s.size() && s[pos] == '-') { sign = -1; pos++; }
    int result = 0;
    while (pos < s.size() && s[pos] >= '0' && s[pos] <= '9') {
        result = result * 10 + (s[pos] - '0');
        pos++;
    }
    return result * sign;
}

static void skipJsonWhitespace(const std::string& s, size_t& pos) {
    while (pos < s.size() && (s[pos] == ' ' || s[pos] == '\n' || s[pos] == '\r' || s[pos] == '\t' || s[pos] == ',')) pos++;
}

static std::vector<std::string> extractJsonStringArray(const std::string& s, size_t& pos) {
    std::vector<std::string> result;
    while (pos < s.size() && s[pos] != '[') pos++;
    if (pos >= s.size()) return result;
    pos++;
    while (pos < s.size()) {
        skipJsonWhitespace(s, pos);
        if (pos >= s.size() || s[pos] == ']') break;
        if (s[pos] == '"') {
            result.push_back(extractJsonString(s, pos));
        } else {
            pos++;
        }
    }
    if (pos < s.size() && s[pos] == ']') pos++;
    return result;
}

static std::vector<int> extractJsonIntArray(const std::string& s, size_t& pos) {
    std::vector<int> result;
    while (pos < s.size() && s[pos] != '[') pos++;
    if (pos >= s.size()) return result;
    pos++;
    while (pos < s.size()) {
        skipJsonWhitespace(s, pos);
        if (pos >= s.size() || s[pos] == ']') break;
        if ((s[pos] >= '0' && s[pos] <= '9') || s[pos] == '-') {
            result.push_back(extractJsonInt(s, pos));
        } else {
            pos++;
        }
    }
    if (pos < s.size() && s[pos] == ']') pos++;
    return result;
}

// ============================================================================
// COMPLETE WORLD BUILDER
// ============================================================================

CompleteWorldBuilder::CompleteWorldBuilder() {}
CompleteWorldBuilder::~CompleteWorldBuilder() { shutdown(); }

bool CompleteWorldBuilder::initialize(SDL2Renderer* renderer, seele::SeeleAIModule* ai) {
    m_renderer = renderer;
    m_ai = ai;
    
    m_world_renderer = new SeeleWorldRenderer();
    if (!m_world_renderer->initialize(renderer, ai)) {
        return false;
    }
    
    m_model_gen = new ModelGenerator();
    m_model_gen->initialize(ai);
    
    return true;
}

void CompleteWorldBuilder::shutdown() {
    delete m_world_renderer;
    m_world_renderer = nullptr;
    delete m_model_gen;
    m_model_gen = nullptr;
}

// ============================================================================
// DISC DEFINITIONS PARSER
// ============================================================================

bool CompleteWorldBuilder::load_disc_definitions(const std::string& json_path) {
    std::ifstream f(json_path);
    if (!f.good()) return false;
    std::stringstream ss;
    ss << f.rdbuf();
    return parse_disc_definitions(ss.str());
}

bool CompleteWorldBuilder::parse_disc_definitions(const std::string& json) {
    // Parse discs array
    size_t pos = json.find("\"discs\"");
    if (pos == std::string::npos) return false;
    pos = json.find('[', pos);
    if (pos == std::string::npos) return false;
    pos++;
    
    while (pos < json.size()) {
        skipJsonWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == ']') break;
        if (json[pos] != '{') { pos++; continue; }
        pos++;
        
        DiscInfo disc;
        while (pos < json.size()) {
            skipJsonWhitespace(json, pos);
            if (pos >= json.size() || json[pos] == '}') { pos++; break; }
            if (json[pos] != '"') { pos++; continue; }
            
            std::string key = extractJsonString(json, pos);
            skipJsonWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ':') pos++;
            skipJsonWhitespace(json, pos);
            if (pos >= json.size()) break;
            
            if (json[pos] == '"') {
                std::string val = extractJsonString(json, pos);
                if (key == "id") disc.id = val;
                else if (key == "title") disc.title = val;
                else if (key == "era") disc.era = val;
                else if (key == "arc") disc.arc = val;
                else if (key == "protagonist") disc.protagonist = val;
                else if (key == "theme_file") disc.theme_file = val;
            } else if (json[pos] == '[') {
                if (key == "zones") {
                    disc.zone_names = extractJsonStringArray(json, pos);
                }
            } else if (json[pos] >= '0' && json[pos] <= '9') {
                int val = extractJsonInt(json, pos);
                if (key == "order") disc.canon_order = val;
            }
        }
        
        if (!disc.id.empty()) {
            std::string disc_id_lower = disc.id;
            for (auto& c : disc_id_lower) c = std::tolower(c);
            disc.theme_file = "/home/sin/workspace/dot-hack-remake/Content/Assets/themes/" + disc_id_lower + "_theme.json";
            m_discs[disc.id] = disc;
        }
    }
    
    // Parse zones array
    return parse_zones(json, "");
}

bool CompleteWorldBuilder::parse_zones(const std::string& json, const std::string& disc_id_filter) {
    size_t pos = json.find("\"zones\"");
    if (pos == std::string::npos) return false;
    pos = json.find('[', pos);
    if (pos == std::string::npos) return false;
    pos++;
    
    while (pos < json.size()) {
        skipJsonWhitespace(json, pos);
        if (pos >= json.size() || json[pos] == ']') break;
        if (json[pos] != '{') { pos++; continue; }
        
        ZoneData zone;
        zone.sky[0] = 25; zone.sky[1] = 5; zone.sky[2] = 10;
        zone.ground[0] = 80; zone.ground[1] = 10; zone.ground[2] = 15;
        zone.fog = 60;
        
        pos++;
        std::string target_disc = disc_id_filter;
        
        while (pos < json.size()) {
            skipJsonWhitespace(json, pos);
            if (pos >= json.size() || json[pos] == '}') { pos++; break; }
            if (json[pos] != '"') { pos++; continue; }
            
            std::string key = extractJsonString(json, pos);
            skipJsonWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ':') pos++;
            skipJsonWhitespace(json, pos);
            if (pos >= json.size()) break;
            
            if (json[pos] == '"') {
                std::string val = extractJsonString(json, pos);
                if (key == "id") zone.id = val;
                else if (key == "name") zone.name = val;
                else if (key == "disc_id") target_disc = val;
                else if (key == "type") zone.type = val;
                else if (key == "mood") zone.mood = val;
                else if (key == "shader") zone.shader = val;
                else if (key == "texture_set") zone.texture_set = val;
                else if (key == "music_track") zone.music_track = val;
            } else if (json[pos] == '[') {
                if (key == "sky") {
                    auto arr = extractJsonIntArray(json, pos);
                    for (size_t i = 0; i < 3 && i < arr.size(); i++) zone.sky[i] = arr[i];
                } else if (key == "ground") {
                    auto arr = extractJsonIntArray(json, pos);
                    for (size_t i = 0; i < 3 && i < arr.size(); i++) zone.ground[i] = arr[i];
                } else if (key == "structures") zone.structures = extractJsonStringArray(json, pos);
                else if (key == "npcs") zone.npcs = extractJsonStringArray(json, pos);
                else if (key == "monsters") zone.monsters = extractJsonStringArray(json, pos);
                else if (key == "items") zone.items = extractJsonStringArray(json, pos);
                else if (key == "avatars") zone.avatars = extractJsonStringArray(json, pos);
            } else if (json[pos] >= '0' && json[pos] <= '9') {
                int val = extractJsonInt(json, pos);
                if (key == "fog") zone.fog = val;
            }
        }
        
        if (!zone.id.empty() && !target_disc.empty()) {
            m_zones[target_disc].push_back(zone);
        }
    }
    
    return !m_zones.empty();
}

// ============================================================================
// THEME PARSER
// ============================================================================

bool CompleteWorldBuilder::load_theme(const std::string& json_path) {
    std::ifstream f(json_path);
    if (!f.good()) return false;
    std::stringstream ss;
    ss << f.rdbuf();
    return parse_theme(ss.str());
}

bool CompleteWorldBuilder::parse_theme(const std::string& json) {
    WorldTheme theme;
    
    size_t pos = json.find("\"theme\"");
    if (pos != std::string::npos) {
        pos = json.find('"', pos + 7);
        if (pos != std::string::npos) theme.disc_id = extractJsonString(json, pos);
    }
    
    // Parse colors
    pos = json.find("\"colors\"");
    if (pos != std::string::npos) {
        pos = json.find('{', pos);
        if (pos != std::string::npos) {
            pos++;
            while (pos < json.size()) {
                skipJsonWhitespace(json, pos);
                if (pos >= json.size() || json[pos] == '}') break;
                if (json[pos] != '"') { pos++; continue; }
                
                std::string key = extractJsonString(json, pos);
                skipJsonWhitespace(json, pos);
                if (pos < json.size() && json[pos] == ':') pos++;
                skipJsonWhitespace(json, pos);
                
                if (pos < json.size() && json[pos] == '"') {
                    std::string val = extractJsonString(json, pos);
                    if (key == "primary") theme.primary_color = val;
                    else if (key == "secondary") theme.secondary_color = val;
                    else if (key == "accent") theme.accent_color = val;
                    else if (key == "background") theme.bg_color = val;
                    else if (key == "text") theme.text_color = val;
                    else if (key == "border") theme.border_color = val;
                }
            }
        }
    }
    
    // Parse effects
    pos = json.find("\"effects\"");
    if (pos != std::string::npos) {
        pos = json.find('{', pos);
        if (pos != std::string::npos) {
            pos++;
            while (pos < json.size()) {
                skipJsonWhitespace(json, pos);
                if (pos >= json.size() || json[pos] == '}') break;
                if (json[pos] != '"') { pos++; continue; }
                
                std::string key = extractJsonString(json, pos);
                skipJsonWhitespace(json, pos);
                if (pos < json.size() && json[pos] == ':') pos++;
                skipJsonWhitespace(json, pos);
                
                if (key == "scanline_opacity" && pos < json.size() && json[pos] >= '0' && json[pos] <= '9') {
                    theme.scanline_intensity = extractJsonInt(json, pos) / 100.0f;
                } else if (key == "glow_intensity" && pos < json.size() && json[pos] >= '0' && json[pos] <= '9') {
                    theme.glow_intensity = extractJsonInt(json, pos) / 100.0f;
                }
                pos++;
            }
        }
    }
    
    if (!theme.disc_id.empty()) {
        m_themes[theme.disc_id] = theme;
    }
    
    return true;
}

// ============================================================================
// WORLD BUILDING
// ============================================================================

bool CompleteWorldBuilder::build_disc(const std::string& disc_id) {
    m_active_disc = disc_id;
    m_active_zone = "";
    
    auto it = m_zones.find(disc_id);
    if (it == m_zones.end()) return false;
    
    // Apply theme if available
    auto theme_it = m_themes.find(disc_id);
    if (theme_it != m_themes.end()) {
        apply_theme(theme_it->second);
    }
    
    // Generate models for this disc
    auto disc_models = m_model_gen->generate_disc_models(disc_id);
    std::printf("[Build] Generated %zu models for disc %s\n", disc_models.size(), disc_id.c_str());
    
    // Build each zone
    for (auto& zone : it->second) {
        generate_disc_structures(disc_id, zone);
        generate_disc_npcs(disc_id, zone);
        generate_disc_monsters(disc_id, zone);
        generate_disc_items(disc_id, zone);
        m_world_renderer->generate_world(disc_id, zone.name);
    }
    
    m_built = true;
    return true;
}

bool CompleteWorldBuilder::build_zone(const std::string& disc_id, const std::string& zone_name) {
    m_active_disc = disc_id;
    m_active_zone = zone_name;
    
    auto it = m_zones.find(disc_id);
    if (it == m_zones.end()) return false;
    
    for (auto& zone : it->second) {
        if (zone.name == zone_name) {
            generate_disc_structures(disc_id, zone);
            generate_disc_npcs(disc_id, zone);
            generate_disc_monsters(disc_id, zone);
            generate_disc_items(disc_id, zone);
            m_world_renderer->generate_world(disc_id, zone_name);
            m_built = true;
            return true;
        }
    }
    
    return false;
}

void CompleteWorldBuilder::build_all() {
    for (auto& [disc_id, zones] : m_zones) {
        build_disc(disc_id);
    }
}

void CompleteWorldBuilder::set_active_disc(const std::string& disc_id) {
    m_active_disc = disc_id;
}

void CompleteWorldBuilder::set_active_zone(const std::string& zone_name) {
    m_active_zone = zone_name;
}

void CompleteWorldBuilder::render_active(float dt) {
    if (m_world_renderer) {
        m_world_renderer->render(dt);
    }
}

DiscInfo CompleteWorldBuilder::get_disc_info(const std::string& disc_id) const {
    auto it = m_discs.find(disc_id);
    if (it != m_discs.end()) return it->second;
    return {};
}

std::vector<std::string> CompleteWorldBuilder::get_all_disc_ids() const {
    std::vector<std::string> ids;
    for (auto& [id, _] : m_discs) ids.push_back(id);
    return ids;
}

std::vector<std::string> CompleteWorldBuilder::get_zone_names(const std::string& disc_id) const {
    std::vector<std::string> names;
    auto it = m_zones.find(disc_id);
    if (it != m_zones.end()) {
        for (auto& z : it->second) names.push_back(z.name);
    }
    return names;
}

void CompleteWorldBuilder::apply_theme(const WorldTheme& theme) {
    // Apply theme colors to world renderer
    // TODO: Apply scanline_intensity, glow_intensity, colors to renderer
}

void CompleteWorldBuilder::generate_disc_structures(const std::string& disc_id, ZoneData& zone) {
    for (const auto& s : zone.structures) {
        if (s == "tower") m_model_gen->generate_model(ModelType::TOWER);
        else if (s == "pillar") m_model_gen->generate_model(ModelType::PILLAR);
        else if (s == "monolith") m_model_gen->generate_model(ModelType::MONOLITH);
        else if (s == "shrine") m_model_gen->generate_model(ModelType::SHRINE);
        else if (s == "gate") m_model_gen->generate_model(ModelType::GATE);
        else if (s == "market") m_model_gen->generate_model(ModelType::MARKET);
        else if (s == "inn") m_model_gen->generate_model(ModelType::INN);
        else if (s == "throne") m_model_gen->generate_model(ModelType::THRONE);
        else if (s == "wall") m_model_gen->generate_model(ModelType::WALL);
        else if (s == "arch") m_model_gen->generate_model(ModelType::ARCH);
        else if (s == "bridge") m_model_gen->generate_model(ModelType::BRIDGE);
        else if (s == "crystal") m_model_gen->generate_model(ModelType::CRYSTAL);
        else if (s == "altar") m_model_gen->generate_model(ModelType::ALTAR);
        else if (s == "cave") m_model_gen->generate_model(ModelType::CAVE);
        else if (s == "spire") m_model_gen->generate_model(ModelType::SPIRE);
        else if (s == "ruin") m_model_gen->generate_model(ModelType::RUIN);
        else if (s == "bone_pile") m_model_gen->generate_model(ModelType::MONSTER_BONE_DRAGON);
        else if (s == "waterfall") m_model_gen->generate_model(ModelType::TREE_WILLOW);
        else if (s == "tree") m_model_gen->generate_model(ModelType::TREE_OAK);
    }
}

void CompleteWorldBuilder::generate_disc_npcs(const std::string& disc_id, ZoneData& zone) {
    for (const auto& npc : zone.npcs) {
        if (npc == "Kite") m_model_gen->generate_model(ModelType::PLAYER_KITE);
        else if (npc == "Haseo") m_model_gen->generate_model(ModelType::PLAYER_HASEO);
        else if (npc == "BlackRose") m_model_gen->generate_model(ModelType::PLAYER_BLACKROSE);
        else if (npc == "Balmung") m_model_gen->generate_model(ModelType::PLAYER_BALMUNG);
        else if (npc == "Tsukasa") m_model_gen->generate_model(ModelType::NPC_TSUKASA);
        else if (npc == "Shugo") m_model_gen->generate_model(ModelType::PLAYER_KITE);
        else if (npc == "Sakuya") m_model_gen->generate_model(ModelType::NPC_SAKUYA);
        else if (npc == "Mimiru") m_model_gen->generate_model(ModelType::NPC_MIMIRU);
        else if (npc == "Subaru") m_model_gen->generate_model(ModelType::NPC_SUBARU);
        else if (npc == "Bear") m_model_gen->generate_model(ModelType::NPC_BEAR);
        else if (npc == "Ovan") m_model_gen->generate_model(ModelType::NPC_OVAN);
        else if (npc == "Atoli") m_model_gen->generate_model(ModelType::NPC_ATOLI);
        else m_model_gen->generate_model(ModelType::NPC_MIMIRU);
    }
}

void CompleteWorldBuilder::generate_disc_monsters(const std::string& disc_id, ZoneData& zone) {
    for (const auto& mon : zone.monsters) {
        if (mon == "Skeleton") m_model_gen->generate_model(ModelType::MONSTER_SKELETON);
        else if (mon == "Corrupted Guard") m_model_gen->generate_model(ModelType::MONSTER_CORRUPTED_GUARD);
        else if (mon == "Data Bug") m_model_gen->generate_model(ModelType::MONSTER_DATA_BUG);
        else if (mon == "Cave Golem") m_model_gen->generate_model(ModelType::MONSTER_CAVE_GOLEM);
        else if (mon == "Shadow Wraith") m_model_gen->generate_model(ModelType::MONSTER_SHADOW_WRAITH);
        else if (mon == "Trial Sentinel") m_model_gen->generate_model(ModelType::MONSTER_TRIAL_SENTINEL);
        else if (mon == "Shadow Knight") m_model_gen->generate_model(ModelType::MONSTER_SHADOW_KNIGHT);
        else if (mon == "AIDA Guardian") m_model_gen->generate_model(ModelType::MONSTER_AIDA_GUARDIAN);
        else if (mon == "Wave Emperor") m_model_gen->generate_model(ModelType::MONSTER_WAVE_EMPEROR);
        else if (mon == "Bone Dragon") m_model_gen->generate_model(ModelType::MONSTER_BONE_DRAGON);
        else if (mon == "Corrupted Sprite") m_model_gen->generate_model(ModelType::MONSTER_CORRUPTED_SPRITE);
        else if (mon == "Shadow Wolf") m_model_gen->generate_model(ModelType::MONSTER_SHADOW_WOLF);
        else if (mon == "Corrupted Treant") m_model_gen->generate_model(ModelType::MONSTER_CORRUPTED_TREANT);
        else if (mon == "Blue Brain") m_model_gen->generate_model(ModelType::MONSTER_BLUE_BRAIN);
        else if (mon == "Shadow Golem") m_model_gen->generate_model(ModelType::MONSTER_SHADOW_GOLEM);
        else if (mon == "Cave Guardian") m_model_gen->generate_model(ModelType::MONSTER_CAVE_GUARDIAN);
        else if (mon == "Ranbabingo") m_model_gen->generate_model(ModelType::MONSTER_RANBABINGO);
        else if (mon == "Dnavarath") m_model_gen->generate_model(ModelType::MONSTER_DNAVARATH);
        else m_model_gen->generate_model(ModelType::MONSTER_SKELETON);
    }
}

void CompleteWorldBuilder::generate_disc_items(const std::string& disc_id, ZoneData& zone) {
    for (const auto& item : zone.items) {
        if (item == "Potion") m_model_gen->generate_model(ModelType::ITEM_POTION);
        else if (item == "Ether") m_model_gen->generate_model(ModelType::ITEM_ETHER);
        else if (item == "Revive") m_model_gen->generate_model(ModelType::ITEM_REVIVE);
        else if (item == "Antidote") m_model_gen->generate_model(ModelType::ITEM_ANTIDOTE);
        else if (item == "Magic Water") m_model_gen->generate_model(ModelType::ITEM_MAGIC_WATER);
        else if (item == "Elixir") m_model_gen->generate_model(ModelType::ITEM_ELIXIR);
        else if (item == "Data Drain") m_model_gen->generate_model(ModelType::ITEM_DATA_DRAIN);
        else if (item == "Skill Book") m_model_gen->generate_model(ModelType::ITEM_SKILL_BOOK);
        else if (item == "Equipment Box") m_model_gen->generate_model(ModelType::ITEM_EQUIPMENT_BOX);
        else if (item == "Carmina Gadelica") m_model_gen->generate_model(ModelType::ITEM_CARMINA_GADELICA);
        else if (item == "Lia Fail") m_model_gen->generate_model(ModelType::ITEM_LIA_FAIL);
        else if (item == "Key") m_model_gen->generate_model(ModelType::ITEM_KEY);
        else m_model_gen->generate_model(ModelType::ITEM_POTION);
    }
}

// ============================================================================
// WORLD3D INTEGRATION
// ============================================================================

World3DIntegration::World3DIntegration() {}
World3DIntegration::~World3DIntegration() { shutdown(); }

bool World3DIntegration::initialize(SDL2Renderer* renderer, seele::SeeleAIModule* ai) {
    m_renderer = renderer;
    m_ai = ai;
    m_builder = new CompleteWorldBuilder();
    return m_builder->initialize(renderer, ai);
}

void World3DIntegration::shutdown() {
    delete m_builder;
    m_builder = nullptr;
}

bool World3DIntegration::load_disc(const std::string& disc_id) {
    if (!m_builder) return false;
    return m_builder->build_disc(disc_id);
}

void World3DIntegration::run() {
    m_running = true;
}

void World3DIntegration::handle_event(const void* event) {
    (void)event;
}

void World3DIntegration::render_frame(float dt) {
    if (m_builder) {
        m_builder->render_active(dt);
    }
}

} // namespace te
