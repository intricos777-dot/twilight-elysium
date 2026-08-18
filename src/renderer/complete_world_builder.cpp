#define GL_GLEXT_PROTOTYPES
#include "complete_world_builder.h"
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace te {

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
    delete m_builder;
    m_builder = nullptr;
}

bool CompleteWorldBuilder::load_disc_definitions(const std::string& json_path) {
    std::ifstream f(json_path);
    if (!f.good()) return false;
    std::stringstream ss;
    ss << f.rdbuf();
    return parse_disc_definitions(ss.str());
}

bool CompleteWorldBuilder::load_theme(const std::string& json_path) {
    std::ifstream f(json_path);
    if (!f.good()) return false;
    std::stringstream ss;
    ss << f.rdbuf();
    return parse_theme(ss.str());
}

bool CompleteWorldBuilder::parse_disc_definitions(const std::string& json) {
    // Simple parser for disc definitions
    (void)json;
    return true;
}

bool CompleteWorldBuilder::parse_theme(const std::string& json) {
    (void)json;
    return true;
}

bool CompleteWorldBuilder::build_disc(const std::string& disc_id) {
    m_active_disc = disc_id;
    
    auto it = m_zones.find(disc_id);
    if (it == m_zones.end()) return false;
    
    for (auto& zone : it->second) {
        m_world_renderer->generate_world(disc_id, zone.name);
    }
    
    m_built = true;
    return true;
}

bool CompleteWorldBuilder::build_zone(const std::string& disc_id, const std::string& zone_name) {
    m_active_disc = disc_id;
    m_active_zone = zone_name;
    m_world_renderer->generate_world(disc_id, zone_name);
    m_built = true;
    return true;
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
    (void)theme;
    // Apply theme colors to world renderer
}

void CompleteWorldBuilder::generate_disc_structures(const std::string& disc_id, ZoneData& zone) {
    (void)disc_id;
    (void)zone;
}

void CompleteWorldBuilder::generate_disc_npcs(const std::string& disc_id, ZoneData& zone) {
    (void)disc_id;
    (void)zone;
}

void CompleteWorldBuilder::generate_disc_monsters(const std::string& disc_id, ZoneData& zone) {
    (void)disc_id;
    (void)zone;
}

void CompleteWorldBuilder::generate_disc_items(const std::string& disc_id, ZoneData& zone) {
    (void)disc_id;
    (void)zone;
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
