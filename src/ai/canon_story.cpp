#include "canon_story.h"
#include <algorithm>
#include <random>

namespace te::hackgu {

CanonStoryGenerator::CanonStoryGenerator() {
    // Load original .hack//Sign / Infection story elements
    m_canon_dialogue["haseo_waking"] = 
        "Haseo: 'This is it... I'm finally going to be strong enough.'";
    
    m_canon_dialogue["haseo_first_contact"] = 
        "Haseo: 'Hey! You there! Are you a player or a monster?'";
    
    m_canon_dialogue["aida_intro"] = 
        "AIDA: 'Designation: AIDA-001. Purpose: Information Analysis and Defense Agent.'";
    
    m_canon_dialogue["sugi_intro"] = 
        "Suguki: 'You shouldn't be here, boy. This area is restricted.'";
    
    m_canon_dialogue["kite_intro"] = 
        "Kite: 'Haseo... you have something I need. Give it to me.'";
    
    // Load characters
    CharacterData haseo;
    haseo.id = "haseo";
    haseo.player_name = "Haseo";
    haseo.guild = "Haseo's Guild";
    haseo.race = "Player";
    haseo.level = 1;
    haseo.title = "The World's Player";
    haseo.stats["hp"] = 100;
    haseo.stats["mp"] = 50;
    haseo.stats["atk"] = 15;
    m_canon_characters["haseo"] = haseo;
    
    CharacterData asuka;
    asuka.id = "asuka";
    asuka.player_name = "Asuka";
    asuka.npc_name = "Asuka Way";
    asuka.guild = "Moonstone";
    asuka.race = "Player";
    asuka.level = 15;
    asuka.title = "Returner";
    asuka.stats["hp"] = 500;
    asuka.stats["mp"] = 250;
    m_canon_characters["asuka"] = asuka;
    
    CharacterData blue_brain;
    blue_brain.id = "blue_brain";
    blue_brain.npc_name = "Blue Brain";
    blue_brain.race = "Monster";
    blue_brain.level = 5;
    m_canon_characters["blue_brain"] = blue_brain;
}

CanonStory CanonStoryGenerator::load_infection_story() {
    CanonStory infection;
    infection.arc = "infection";
    infection.volume = 1;
    infection.name = "INFECITON // First Contact";
    
    // Disc 1: Awakening
    StoryNode disc1_node1;
    disc1_node1.id = "disc1_node1";
    disc1_node1.title = "Awakening";
    disc1_node1.description = "Haseo awakens in the Moon Tree, unaware of the infection spreading.";
    disc1_node1.speaker = "Narrator";
    disc1_node1.choices = {"Enter the data realm", "Observe the surroundings", "Call out for help"};
    infection.nodes.push_back(disc1_node1);
    
    StoryNode disc1_node2;
    disc1_node2.id = "disc1_node2";
    disc1_node2.title = "First Contact";
    disc1_node2.description = "Haseo encounters his first monster - a Blue Brain in a data storm.";
    disc1_node2.speaker = "Haseo";
    disc1_node2.description = "Haseo: 'Hey! You there! Are you a player or a monster?'";
    disc1_node2.consequences = {"Combat initiated", "Player learns monster behavior", "Data storm intensifies"};
    infection.nodes.push_back(disc1_node2);
    
    // Quests for Infection
    QuestData quest1;
    quest1.id = "inf_quest_001";
    quest1.title = "Clear the Data Storm";
    quest1.description = "Clear the corrupted data storm in the Moon Tree area.";
    quest1.objectives = {"Find the source of corruption", "Defeat 5 Blue Brains", "Seal the data rift"};
    quest1.rewards = {"EXP +50", "Ryuken +1", "Item: Healing Potion x2"};
    quest1.giver = "System";
    quest1.location = "Moon Tree";
    quest1.level = 1;
    infection.quests.push_back(quest1);
    
    CharacterData haseo_disc1;
    haseo_disc1 = m_canon_characters["haseo"];
    haseo_disc1.level = 2;
    infection.characters.push_back(haseo_disc1);
    
    // World events
    WorldEvent event1;
    event1.trigger_id = "data_storm";
    event1.description = "Data storms increase enemy spawn rates";
    event1.effect_type = "spawn";
    event1.target = "Blue Brain";
    event1.magnitude = 2.0f;
    infection.events.push_back(event1);
    
    return infection;
}

CanonStory CanonStoryGenerator::load_gu_story() {
    CanonStory gu;
    gu.arc = "gu";
    gu.volume = 1;
    gu.name = "G.U. // Returner Arc";
    
    // Volume 1: Player
    StoryNode vol1_start;
    vol1_start.id = "gu_vol1_start";
    vol1_start.title = "Mac Anu Awakening";
    vol1_start.description = "Asuka awakens in Mac Anu with fragmented memories of being a Returner.";
    vol1_start.speaker = "Asuka";
    vol1_start.description = "Asuka: 'Where... am I? Why do I remember being someone else?'";
    gu.nodes.push_back(vol1_start);
    
    StoryNode vol1_meet_kite;
    vol1_meet_kite.id = "gu_vol1_meet_kite";
    vol1_meet_kite.title = "The Returner's Past";
    vol1_meet_kite.description = "Kite reveals Asuka's connection to the Returner project.";
    vol1_meet_kite.speaker = "Kite";
    vol1_meet_kite.description = "Kite: 'You are connected to the Returner. This is why they want you.'</";
    vol1_meet_kite.flags["kite_known"] = "true";
    gu.nodes.push_back(vol1_meet_kite);
    
    // Quests for GU Volume 1
    QuestData gu_quest1;
    gu_quest1.id = "gu_quest_001";
    gu_quest1.title = "Retrieve Data Fragments";
    gu_quest1.description = "Collect 3 data fragments scattered in Mac Anu to restore memories.";
    gu_quest1.objectives = {"Search Terminal Area", "Find Fragment A", "Find Fragment B", "Find Fragment C"};
    gu_quest1.rewards = {"EXP +100", "Data Blade x1", "Title: Returning Soul"};
    gu_quest1.giver = "Kite";
    gu_quest1.location = "Mac Anu";
    gu_quest1.level = 15;
    gu.quests.push_back(gu_quest1);
    
    // Characters
    CharacterData asuka_vol1 = m_canon_characters["asuka"];
    asuka_vol1.level = 15;
    gu.characters.push_back(asuka_vol1);
    
    CharacterData kite;
    kite.id = "kite";
    kite.npc_name = "Kite";
    kite.title = "The World's Guardian";
    kite.stats["hp"] = 800;
    kite.stats["mp"] = 500;
    gu.characters.push_back(kite);
    
    return gu;
}

CanonStory CanonStoryGenerator::generate_infection_full() {
    CanonStory full;
    full.arc = "infection";
    full.name = "INFECITON // Complete Arc";
    
    // Load all 4 discs
    for (int i = 1; i <= 4; i++) {
        CanonStory disc = load_infection_story();
        disc.volume = i;
        full.nodes.insert(full.nodes.end(), disc.nodes.begin(), disc.nodes.end());
        full.quests.insert(full.quests.end(), disc.quests.begin(), disc.quests.end());
        full.events.insert(full.events.end(), disc.events.begin(), disc.events.end());
    }
    
    return full;
}

CanonStory CanonStoryGenerator::generate_gu_full() {
    CanonStory full;
    full.arc = "gu";
    full.name = "G.U. // Complete Arc";
    
    for (int i = 1; i <= 4; i++) {
        CanonStory vol = load_gu_story();
        vol.volume = i;
        full.nodes.insert(full.nodes.end(), vol.nodes.begin(), vol.nodes.end());
        full.quests.insert(full.quests.end(), vol.quests.begin(), vol.quests.end());
    }
    
    return full;
}

void CanonStoryGenerator::alter_story_node(CanonStory& story, const std::string& node_id,
                                            const std::string& new_text, int delay_ms) {
    for (auto& node : story.nodes) {
        if (node.id == node_id) {
            node.description = "[AIDA ALTERED]\n" + new_text;
            // Mark as altered for save state
            node.flags["altered_by_aida"] = "true";
            break;
        }
    }
}

void CanonStoryGenerator::insert_story_branch(CanonStory& story, const std::string& node_id,
                                               const std::string& new_node_id) {
    StoryNode branch;
    branch.id = new_node_id;
    branch.title = "AIDA Intervention";
    branch.description = "AIDA has altered the flow of events in this area.";
    branch.speaker = "AIDA";
    branch.choices = {"Proceed with altered path", "Return to original"};
    
    story.nodes.push_back(branch);
}

void CanonStoryGenerator::remove_story_node(CanonStory& story, const std::string& node_id) {
    story.nodes.erase(
        std::remove_if(story.nodes.begin(), story.nodes.end(),
            [&node_id](const StoryNode& n) { return n.id == node_id; }),
        story.nodes.end()
    );
}

void CanonStoryGenerator::modify_character(CanonStory& story, const std::string& character_id,
                                            const std::string& new_line) {
    for (auto& char_data : story.characters) {
        if (char_data.id == character_id) {
            // Add new dialogue option
            char_data.stats["aida_influence"] = 
                (char_data.stats.count("aida_influence") ? char_data.stats["aida_influence"] : 0) + 1.0f;
            break;
        }
    }
}

std::string CanonStoryGenerator::get_node_text(const CanonStory& story, const std::string& node_id) {
    for (const auto& node : story.nodes) {
        if (node.id == node_id) {
            return node.description;
        }
    }
    return "";
}

} // namespace te::hackgu