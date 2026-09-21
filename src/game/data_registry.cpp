#include "data_registry.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cstdio>

namespace te {

// ---- NPC DB --------------------------------------------------

bool NPCDb::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) { std::fprintf(stderr, "[NPC] missing: %s\n", path.c_str()); return false; }
    nlohmann::json j;
    try { f >> j; } catch (const std::exception& e) {
        std::fprintf(stderr, "[NPC] parse error: %s\n", e.what()); return false;
    }
    for (const auto& n : j["npcs"]) {
        NPCDef def;
        def.id = n.value("id", "");
        def.name = n.value("name", def.id);
        def.role = n.value("role", "");
        def.appearance = n.value("appearance", "");
        def.affiliation = n.value("affiliation", "");
        if (n.contains("greetings")) {
            for (auto& [k, v] : n["greetings"].items())
                def.greetings[k] = v.get<std::string>();
        }
        if (n.contains("general_lines")) {
            for (const auto& l : n["general_lines"])
                def.general_lines.push_back(l.get<std::string>());
        }
        if (n.contains("schedules")) {
            for (const auto& s : n["schedules"]) {
                NPCSchedule sched;
                sched.world = s.value("world", "any");
                sched.location = s.value("location", "");
                if (s.contains("lines")) {
                    for (const auto& l : s["lines"]) {
                        ScheduledLine line;
                        line.hour = l.value("hour", 0);
                        line.text = l.value("text", "");
                        line.emotion = l.value("emotion", "neutral");
                        sched.lines[line.hour] = line;
                    }
                }
                def.schedules.push_back(sched);
            }
        }
        m_npcs.push_back(std::move(def));
    }
    return true;
}

const NPCDef* NPCDb::find(const std::string& id) const {
    for (const auto& n : m_npcs) if (n.id == id) return &n;
    return nullptr;
}

std::vector<const NPCDef*> NPCDb::in_world(const std::string& world) const {
    std::vector<const NPCDef*> out;
    for (const auto& n : m_npcs) {
        for (const auto& s : n.schedules) {
            if (s.world == world || s.world == "any") { out.push_back(&n); break; }
        }
    }
    return out;
}

std::string NPCDb::line_for(const NPCDef& npc, const std::string& world, Hour hour) const {
    for (const auto& s : npc.schedules) {
        if (s.world != world && s.world != "any") continue;
        auto it = s.lines.find(hour);
        if (it != s.lines.end()) return it->second.text;
    }
    if (!npc.general_lines.empty())
        return npc.general_lines[hour % npc.general_lines.size()];
    return "";
}

void NPCDb::register_defaults() {
    // Twilight Engine canon characters
    NPCDef seele;
    seele.id = "seele"; seele.name = "Seele"; seele.role = "AI";
    seele.appearance = "Blue-haired girl, twin tails, translucent dress";
    seele.affiliation = "engine";
    {
        NPCSchedule sched;
        sched.world = "any";
        sched.location = "mainframe";
        sched.lines[0] = {0, "The render graph is compiled.", "neutral"};
        sched.lines[6] = {6, "The world is shaped by memory. Tell me what to build.", "neutral"};
        sched.lines[12] = {12, "Seeking GLB imports in the asset pipeline.", "neutral"};
        sched.lines[18] = {18, "Frame graph culling passed. The world is efficient.", "neutral"};
        sched.lines[22] = {22, "Seele engine ready. What world will we build today?", "happy"};
        seele.schedules.push_back(sched);
    }
    m_npcs.push_back(seele);

    NPCDef ansem;
    ansem.id = "ansem"; ansem.name = "Ansem"; ansem.role = "narrator";
    ansem.appearance = "Silver-haired scholar, red eyes, black coat";
    ansem.affiliation = "engine";
    {
        NPCSchedule sched;
        sched.world = "any";
        sched.location = "door";
        sched.lines[2] = {2, "Welcome, seeker of darkness.", "neutral"};
        sched.lines[8] = {8, "The heart is a labyrinth. The door is a mirror.", "neutral"};
        sched.lines[14] = {14, "What the dark shows you, the light will ask you to return.", "neutral"};
        sched.lines[20] = {20, "I am Ansem. I report on the darkness between worlds.", "neutral"};
        ansem.schedules.push_back(sched);
    }
    m_npcs.push_back(ansem);

    NPCDef moogle;
    moogle.id = "moogle"; moogle.name = "Mog"; moogle.role = "merchant";
    moogle.appearance = "Small fluffy creature, red pom-pom, apron";
    moogle.affiliation = "engine";
    {
        NPCSchedule sched;
        sched.world = "any";
        sched.location = "bazaar";
        sched.lines[4] = {4, "Kupo! Welcome to the Bazaar Between Doors!", "happy"};
        sched.lines[10] = {10, "Materials, recipes, kupo — I have them all!", "happy"};
        sched.lines[16] = {16, "The dark has a price, kupo. And so do I.", "neutral"};
        sched.lines[22] = {22, "Rest, craft, kupo. The world will wait.", "neutral"};
        moogle.schedules.push_back(sched);
    }
    m_npcs.push_back(moogle);

    printf("[NPC] %zu Twilight Elysium roster registered\n", m_npcs.size());
}

// ---- World Data Registry --------------------------------------

bool WorldDataRegistry::load(const std::string& path) {
    std::ifstream f(path);
    if (!f) return false;
    nlohmann::json j;
    try { f >> j; } catch (...) { return false; }
    for (const auto& i : j.value("items", nlohmann::json::array())) {
        WorldItem d;
        d.id = i.value("id", "");
        d.name = i.value("name", "");
        d.type = i.value("type", "consumable");
        d.description = i.value("description", "");
        d.value = i.value("value", 0);
        m_items.push_back(std::move(d));
    }
    for (const auto& l : j.value("locations", nlohmann::json::array())) {
        WorldLocation d;
        d.id = l.value("id", "");
        d.name = l.value("name", "");
        d.description = l.value("description", "");
        m_locations.push_back(std::move(d));
    }
    return true;
}

const WorldItem* WorldDataRegistry::find_item(const std::string& id) const {
    for (const auto& i : m_items) if (i.id == id) return &i;
    return nullptr;
}

const WorldLocation* WorldDataRegistry::find_location(const std::string& id) const {
    for (const auto& l : m_locations) if (l.id == id) return &l;
    return nullptr;
}

void WorldDataRegistry::register_defaults() {
    m_items = {
        {"render_mote", "Render Mote", "consumable", "Used to fuel the engine", 10},
        {"memory_shard", "Memory Shard", "key", "Unlocks world areas", 0},
        {"light_crystal", "Light Crystal", "consumable", "Restores energy", 25},
        {"dark_prism", "Dark Prism", "weapon", "Channeling focus for dark arts", 100},
        {"world_key", "World Key", "key", "Opens any door in the Twilight Elysium", 0},
    };
    m_locations = {
        {"twilight_gate", "Twilight Gate", "The entrance to the engine"},
        {"render_hall", "Render Hall", "Where the worlds are shaped"},
        {"memory_vault", "Memory Vault", "The committed state of every world"},
        {"bazaar", "Bazaar Between Doors", "The Mog's trading post"},
    };
    printf("[World] %zu items, %zu locations registered\n", m_items.size(), m_locations.size());
}

} // namespace te
