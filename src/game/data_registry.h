#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace te {

// Time-of-day in an in-game world (0-23 hours)
using Hour = uint32_t;

// A scheduled line of dialogue an NPC says at a given time in a given world.
struct ScheduledLine {
    Hour hour = 0;
    std::string text;
    std::string emotion;
};

// An NPC's full schedule: where they are and what they say, keyed by world.
struct NPCSchedule {
    std::string world;
    std::map<Hour, ScheduledLine> lines;
    std::string location;
};

// A named NPC character.
struct NPCDef {
    std::string id;
    std::string name;
    std::string role;
    std::string appearance;
    std::string affiliation;
    std::vector<NPCSchedule> schedules;
    std::map<std::string, std::string> greetings;
    std::vector<std::string> general_lines;
};

// Data registry for NPCs loaded from JSON or hardcoded defaults
class NPCDb {
public:
    bool load(const std::string& path);

    const std::vector<NPCDef>& npcs() const { return m_npcs; }
    const NPCDef* find(const std::string& id) const;
    std::vector<const NPCDef*> in_world(const std::string& world) const;

    std::string line_for(const NPCDef& npc, const std::string& world, Hour hour) const;

    void register_defaults();

private:
    std::vector<NPCDef> m_npcs;
};

// World content registry — items, locations, enemy spawns
struct WorldItem {
    std::string id;
    std::string name;
    std::string type;  // "weapon", "armor", "consumable", "key"
    std::string description;
    uint32_t value = 0;
};

struct WorldLocation {
    std::string id;
    std::string name;
    std::string description;
    std::vector<std::string> npcs;
    std::vector<std::string> items;
};

class WorldDataRegistry {
public:
    bool load(const std::string& path);

    const std::vector<WorldItem>& items() const { return m_items; }
    const std::vector<WorldLocation>& locations() const { return m_locations; }
    const WorldItem* find_item(const std::string& id) const;
    const WorldLocation* find_location(const std::string& id) const;

    void register_defaults();

private:
    std::vector<WorldItem> m_items;
    std::vector<WorldLocation> m_locations;
};

} // namespace te
