#pragma once

#include <string>
#include <vector>
#include <map>

namespace te::hackgu {

// ============================================================================
// CANON STORY DATA STRUCTURES
// ============================================================================

struct StoryNode {
    std::string id;
    std::string title;
    std::string description;
    std::string speaker;
    std::string audio_file;
    float duration = 0.0f;
    std::vector<std::string> choices;  // Decision points
    std::vector<std::string> consequences;
    std::map<std::string, std::string> flags;  // State changes
};

struct QuestData {
    std::string id;
    std::string title;
    std::string description;
    std::vector<std::string> objectives;
    std::vector<std::string> rewards;
    std::string giver;
    std::string location;
    int level = 1;
    float corruption = 0.0f;
};

struct CharacterData {
    std::string id;
    std::string player_name;
    std::string npc_name;
    std::string guild;
    std::string race;
    int level = 1;
    std::string title;
    std::vector<std::string> equipment;
    std::map<std::string, float> stats;
};

struct WorldEvent {
    std::string trigger_id;
    std::string description;
    std::string effect_type;  // "spawn", "damage", "dialogue", "transition"
    std::string target;
    float magnitude = 1.0f;
};

struct CanonStory {
    std::string arc;              // "infection" or "gu"
    int volume;                   // 1-4
    std::string name;
    std::vector<StoryNode> nodes;
    std::vector<QuestData> quests;
    std::vector<CharacterData> characters;
    std::vector<WorldEvent> events;
    std::map<std::string, std::string> environment_info;
};

// ============================================================================
// CANON STORY GENERATOR - Original .hack//G.U. and Infection content
// ============================================================================

class CanonStoryGenerator {
public:
    CanonStoryGenerator();
    ~CanonStoryGenerator() = default;
    
    // Load original canon story for Infection arc
    CanonStory load_infection_story();
    
    // Load original canon story for G.U. arc
    CanonStory load_gu_story();
    
    // Generate full disk 1-4 content for Infection
    CanonStory generate_infection_full();
    
    // Generate full volume 1-4 content for G.U.
    CanonStory generate_gu_full();
    
    // Story alteration methods for AIDA
    void alter_story_node(CanonStory& story, const std::string& node_id, 
                          const std::string& new_text, int delay_ms = 5000);
    
    void insert_story_branch(CanonStory& story, const std::string& node_id,
                             const std::string& new_node_id);
    
    void remove_story_node(CanonStory& story, const std::string& node_id);
    
    void modify_character(CanonStory& story, const std::string& character_id,
                          const std::string& new_line);
    
    // Get story by key
    std::string get_node_text(const CanonStory& story, const std::string& node_id);
    
private:
    std::map<std::string, std::string> m_canon_dialogue;
    std::map<std::string, std::vector<std::string>> m_canon_quests;
    std::map<std::string, CharacterData> m_canon_characters;
};

} // namespace te::hackgu