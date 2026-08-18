#define GL_GLEXT_PROTOTYPES
#include "model_generator.h"
#include <GL/gl.h>
#include <cmath>
#include <cstdio>

namespace te {

ModelGenerator::ModelGenerator() {}
ModelGenerator::~ModelGenerator() {}

void ModelGenerator::initialize(seele::SeeleAIModule* ai) {
    m_ai = ai;
}

ModelParams ModelGenerator::default_params(ModelType type) {
    ModelParams p;
    switch (type) {
        case ModelType::TOWER: p.height = 8.0f; p.width = 1.5f; break;
        case ModelType::PILLAR: p.height = 5.0f; p.width = 0.5f; break;
        case ModelType::MONOLITH: p.height = 6.0f; p.width = 2.0f; break;
        case ModelType::SHRINE: p.height = 4.0f; p.width = 0.8f; break;
        case ModelType::GATE: p.height = 5.0f; p.width = 3.0f; break;
        case ModelType::MARKET: p.height = 3.0f; p.width = 5.0f; break;
        case ModelType::INN: p.height = 4.0f; p.width = 6.0f; break;
        case ModelType::THRONE: p.height = 4.0f; p.width = 2.0f; break;
        case ModelType::WALL: p.height = 3.0f; p.width = 0.5f; break;
        case ModelType::ARCH: p.height = 4.0f; p.width = 2.0f; break;
        case ModelType::BRIDGE: p.height = 0.5f; p.width = 2.0f; break;
        case ModelType::CRYSTAL: p.height = 3.0f; p.width = 0.8f; break;
        case ModelType::ALTAR: p.height = 2.0f; p.width = 1.5f; break;
        case ModelType::CAVE: p.height = 3.0f; p.width = 4.0f; break;
        case ModelType::SPIRE: p.height = 10.0f; p.width = 0.5f; break;
        case ModelType::RUIN: p.height = 2.0f; p.width = 3.0f; break;
        case ModelType::PLAYER_KITE: p.height = 1.8f; p.width = 0.4f; p.depth = 0.25f; break;
        case ModelType::PLAYER_HASEO: p.height = 1.85f; p.width = 0.45f; p.depth = 0.3f; break;
        case ModelType::PLAYER_BLACKROSE: p.height = 1.7f; p.width = 0.4f; p.depth = 0.25f; break;
        case ModelType::PLAYER_BALMUNG: p.height = 1.9f; p.width = 0.5f; p.depth = 0.35f; break;
        case ModelType::MONSTER_SKELETON: p.height = 1.7f; p.width = 0.4f; p.depth = 0.25f; break;
        case ModelType::MONSTER_CORRUPTED_GUARD: p.height = 2.0f; p.width = 0.6f; p.depth = 0.4f; break;
        case ModelType::MONSTER_DATA_BUG: p.height = 0.3f; p.width = 0.3f; p.depth = 0.3f; break;
        case ModelType::MONSTER_CAVE_GOLEM: p.height = 3.0f; p.width = 1.5f; p.depth = 1.2f; break;
        case ModelType::MONSTER_SHADOW_WRAITH: p.height = 2.0f; p.width = 0.8f; p.depth = 0.6f; break;
        case ModelType::MONSTER_TRIAL_SENTINEL: p.height = 3.5f; p.width = 2.0f; p.depth = 2.0f; break;
        case ModelType::MONSTER_SHADOW_KNIGHT: p.height = 2.2f; p.width = 0.7f; p.depth = 0.5f; break;
        case ModelType::MONSTER_AIDA_GUARDIAN: p.height = 4.0f; p.width = 2.5f; p.depth = 2.0f; break;
        case ModelType::MONSTER_WAVE_EMPEROR: p.height = 5.0f; p.width = 3.0f; p.depth = 2.5f; break;
        case ModelType::MONSTER_BONE_DRAGON: p.height = 6.0f; p.width = 8.0f; p.depth = 4.0f; break;
        case ModelType::MONSTER_CORRUPTED_SPRITE: p.height = 0.5f; p.width = 0.5f; p.depth = 0.5f; break;
        case ModelType::MONSTER_SHADOW_WOLF: p.height = 1.2f; p.width = 0.5f; p.depth = 1.5f; break;
        case ModelType::MONSTER_CORRUPTED_TREANT: p.height = 5.0f; p.width = 2.0f; p.depth = 2.0f; break;
        case ModelType::MONSTER_BLUE_BRAIN: p.height = 0.8f; p.width = 1.0f; p.depth = 0.8f; break;
        case ModelType::MONSTER_SHADOW_GOLEM: p.height = 4.0f; p.width = 2.0f; p.depth = 1.8f; break;
        case ModelType::MONSTER_CAVE_GUARDIAN: p.height = 4.5f; p.width = 2.5f; p.depth = 2.0f; break;
        case ModelType::MONSTER_RANBABINGO: p.height = 2.0f; p.width = 1.5f; p.depth = 1.5f; break;
        case ModelType::MONSTER_DNAVARATH: p.height = 3.0f; p.width = 2.0f; p.depth = 2.0f; break;
        case ModelType::ITEM_POTION: p.height = 0.3f; p.width = 0.1f; p.depth = 0.1f; break;
        case ModelType::ITEM_ETHER: p.height = 0.3f; p.width = 0.1f; p.depth = 0.1f; break;
        case ModelType::ITEM_REVIVE: p.height = 0.4f; p.width = 0.15f; p.depth = 0.15f; break;
        case ModelType::ITEM_ANTIDOTE: p.height = 0.3f; p.width = 0.1f; p.depth = 0.1f; break;
        case ModelType::ITEM_MAGIC_WATER: p.height = 0.4f; p.width = 0.15f; p.depth = 0.15f; break;
        case ModelType::ITEM_ELIXIR: p.height = 0.5f; p.width = 0.2f; p.depth = 0.2f; break;
        case ModelType::ITEM_DATA_DRAIN: p.height = 0.4f; p.width = 0.2f; p.depth = 0.2f; break;
        case ModelType::ITEM_SKILL_BOOK: p.height = 0.3f; p.width = 0.2f; p.depth = 0.05f; break;
        case ModelType::ITEM_EQUIPMENT_BOX: p.height = 0.5f; p.width = 0.5f; p.depth = 0.5f; break;
        case ModelType::ITEM_CARMINA_GADELICA: p.height = 1.5f; p.width = 0.15f; p.depth = 0.1f; break;
        case ModelType::ITEM_LIA_FAIL: p.height = 1.8f; p.width = 0.2f; p.depth = 0.15f; break;
        case ModelType::ITEM_KEY: p.height = 0.2f; p.width = 0.08f; p.depth = 0.04f; break;
        case ModelType::WEAPON_DUAL_SWORDS: p.height = 1.2f; p.width = 0.15f; p.depth = 0.05f; break;
        case ModelType::WEAPON_HEAVY_BLADE: p.height = 1.5f; p.width = 0.3f; p.depth = 0.1f; break;
        case ModelType::WEAPON_STAFF: p.height = 1.6f; p.width = 0.08f; p.depth = 0.08f; break;
        case ModelType::WEAPON_SPEAR: p.height = 1.8f; p.width = 0.1f; p.depth = 0.1f; break;
        case ModelType::WEAPON_DAGGER: p.height = 0.5f; p.width = 0.08f; p.depth = 0.03f; break;
        case ModelType::WEAPON_SHIELD: p.height = 0.8f; p.width = 0.6f; p.depth = 0.1f; break;
        case ModelType::WEAPON_RYUKEN: p.height = 1.0f; p.width = 0.2f; p.depth = 0.08f; break;
        case ModelType::WEAPON_AVATAR_KITE: p.height = 2.0f; p.width = 0.4f; p.depth = 0.2f; break;
        case ModelType::AVATAR_TSUKUYOMI: p.height = 6.0f; p.width = 3.0f; p.depth = 2.5f; break;
        case ModelType::AVATAR_KITE: p.height = 5.0f; p.width = 2.5f; p.depth = 2.0f; break;
        case ModelType::AVATAR_HASEO_5TH: p.height = 7.0f; p.width = 3.5f; p.depth = 3.0f; break;
        case ModelType::AVATAR_AZURE_KITE: p.height = 5.5f; p.width = 2.8f; p.depth = 2.2f; break;
        case ModelType::TREE_WILLOW: p.height = 6.0f; p.width = 3.0f; p.depth = 3.0f; break;
        case ModelType::TREE_OAK: p.height = 8.0f; p.width = 4.0f; p.depth = 4.0f; break;
        case ModelType::TREE_MAPLE: p.height = 7.0f; p.width = 3.5f; p.depth = 3.5f; break;
        case ModelType::TREE_BAMBOO: p.height = 10.0f; p.width = 0.3f; p.depth = 0.3f; break;
        case ModelType::TREE_CEDAR: p.height = 12.0f; p.width = 2.0f; p.depth = 2.0f; break;
        case ModelType::TREE_PINE: p.height = 10.0f; p.width = 2.5f; p.depth = 2.5f; break;
        case ModelType::GRUNTY_MOUNT: p.height = 2.0f; p.width = 1.0f; p.depth = 2.5f; break;
        case ModelType::DATA_MOTE: p.height = 0.2f; p.width = 0.2f; p.depth = 0.2f; break;
        case ModelType::SKY_GRID: p.height = 0.0f; p.width = 200.0f; p.depth = 200.0f; break;
        default: break;
    }
    return p;
}

std::string ModelGenerator::model_name(ModelType type) {
    switch (type) {
        case ModelType::TOWER: return "tower";
        case ModelType::PILLAR: return "pillar";
        case ModelType::MONOLITH: return "monolith";
        case ModelType::SHRINE: return "shrine";
        case ModelType::GATE: return "gate";
        case ModelType::MARKET: return "market";
        case ModelType::INN: return "inn";
        case ModelType::THRONE: return "throne";
        case ModelType::WALL: return "wall";
        case ModelType::ARCH: return "arch";
        case ModelType::BRIDGE: return "bridge";
        case ModelType::CRYSTAL: return "crystal";
        case ModelType::ALTAR: return "altar";
        case ModelType::CAVE: return "cave";
        case ModelType::SPIRE: return "spire";
        case ModelType::RUIN: return "ruin";
        case ModelType::PLAYER_KITE: return "player_kite";
        case ModelType::PLAYER_HASEO: return "player_haseo";
        case ModelType::PLAYER_BLACKROSE: return "player_blackrose";
        case ModelType::PLAYER_BALMUNG: return "player_balung";
        case ModelType::MONSTER_SKELETON: return "skeleton";
        case ModelType::MONSTER_CORRUPTED_GUARD: return "corrupted_guard";
        case ModelType::MONSTER_DATA_BUG: return "data_bug";
        case ModelType::MONSTER_CAVE_GOLEM: return "cave_golem";
        case ModelType::MONSTER_SHADOW_WRAITH: return "shadow_wraith";
        case ModelType::MONSTER_TRIAL_SENTINEL: return "trial_sentinel";
        case ModelType::MONSTER_SHADOW_KNIGHT: return "shadow_knight";
        case ModelType::MONSTER_AIDA_GUARDIAN: return "aida_guardian";
        case ModelType::MONSTER_WAVE_EMPEROR: return "wave_emperor";
        case ModelType::MONSTER_BONE_DRAGON: return "bone_dragon";
        case ModelType::MONSTER_CORRUPTED_SPRITE: return "corrupted_sprite";
        case ModelType::MONSTER_SHADOW_WOLF: return "shadow_wolf";
        case ModelType::MONSTER_CORRUPTED_TREANT: return "corrupted_treant";
        case ModelType::MONSTER_BLUE_BRAIN: return "blue_brain";
        case ModelType::MONSTER_SHADOW_GOLEM: return "shadow_golem";
        case ModelType::MONSTER_CAVE_GUARDIAN: return "cave_guardian";
        case ModelType::MONSTER_RANBABINGO: return "ranbabingo";
        case ModelType::MONSTER_DNAVARATH: return "dnavarath";
        case ModelType::ITEM_POTION: return "potion";
        case ModelType::ITEM_ETHER: return "ether";
        case ModelType::ITEM_REVIVE: return "revive";
        case ModelType::ITEM_ANTIDOTE: return "antidote";
        case ModelType::ITEM_MAGIC_WATER: return "magic_water";
        case ModelType::ITEM_ELIXIR: return "elixir";
        case ModelType::ITEM_DATA_DRAIN: return "data_drain";
        case ModelType::ITEM_SKILL_BOOK: return "skill_book";
        case ModelType::ITEM_EQUIPMENT_BOX: return "equipment_box";
        case ModelType::ITEM_CARMINA_GADELICA: return "carmina_gadelica";
        case ModelType::ITEM_LIA_FAIL: return "lia_fail";
        case ModelType::ITEM_KEY: return "key";
        case ModelType::WEAPON_DUAL_SWORDS: return "dual_swords";
        case ModelType::WEAPON_HEAVY_BLADE: return "heavy_blade";
        case ModelType::WEAPON_STAFF: return "staff";
        case ModelType::WEAPON_SPEAR: return "spear";
        case ModelType::WEAPON_DAGGER: return "dagger";
        case ModelType::WEAPON_SHIELD: return "shield";
        case ModelType::WEAPON_RYUKEN: return "ryuken";
        case ModelType::WEAPON_AVATAR_KITE: return "avatar_weapon_kite";
        case ModelType::AVATAR_TSUKUYOMI: return "avatar_tsukuyomi";
        case ModelType::AVATAR_KITE: return "avatar_kite";
        case ModelType::AVATAR_HASEO_5TH: return "avatar_haseo_5th";
        case ModelType::AVATAR_AZURE_KITE: return "avatar_azure_kite";
        case ModelType::TREE_WILLOW: return "tree_willow";
        case ModelType::TREE_OAK: return "tree_oak";
        case ModelType::TREE_MAPLE: return "tree_maple";
        case ModelType::TREE_BAMBOO: return "tree_bamboo";
        case ModelType::TREE_CEDAR: return "tree_cedar";
        case ModelType::TREE_PINE: return "tree_pine";
        case ModelType::GRUNTY_MOUNT: return "grunty";
        case ModelType::DATA_MOTE: return "data_mote";
        case ModelType::SKY_GRID: return "sky_grid";
        default: return "unknown";
    }
}

WorldMesh* ModelGenerator::generate_model(ModelType type, const ModelParams& params) {
    // Check if already generated
    auto it = m_models.find(type);
    if (it != m_models.end()) return it->second;
    
    ModelParams p = params.scale > 0 ? params : default_params(type);
    
    WorldMesh* mesh = nullptr;
    
    switch (type) {
        // Structures
        case ModelType::TOWER: mesh = gen_tower(p); break;
        case ModelType::PILLAR: mesh = gen_pillar(p); break;
        case ModelType::MONOLITH: mesh = gen_monolith(p); break;
        case ModelType::SHRINE: mesh = gen_shrine(p); break;
        case ModelType::GATE: mesh = gen_gate(p); break;
        case ModelType::MARKET: mesh = gen_market(p); break;
        case ModelType::INN: mesh = gen_inn(p); break;
        case ModelType::THRONE: mesh = gen_throne(p); break;
        case ModelType::WALL: mesh = gen_wall(p); break;
        case ModelType::ARCH: mesh = gen_arch(p); break;
        case ModelType::BRIDGE: mesh = gen_bridge(p); break;
        case ModelType::CRYSTAL: mesh = gen_crystal(p); break;
        case ModelType::ALTAR: mesh = gen_altar(p); break;
        case ModelType::CAVE: mesh = gen_cave(p); break;
        case ModelType::SPIRE: mesh = gen_spire(p); break;
        case ModelType::RUIN: mesh = gen_ruin(p); break;
        
        // Characters
        case ModelType::PLAYER_KITE: mesh = gen_player_kite(p); break;
        case ModelType::PLAYER_HASEO: mesh = gen_player_haseo(p); break;
        case ModelType::PLAYER_BLACKROSE: mesh = gen_player_blackrose(p); break;
        case ModelType::PLAYER_BALMUNG: mesh = gen_player_balung(p); break;
        
        // Monsters
        case ModelType::MONSTER_SKELETON: mesh = gen_skeleton(p); break;
        case ModelType::MONSTER_CORRUPTED_GUARD: mesh = gen_corrupted_guard(p); break;
        case ModelType::MONSTER_DATA_BUG: mesh = gen_data_bug(p); break;
        case ModelType::MONSTER_CAVE_GOLEM: mesh = gen_cave_golem(p); break;
        case ModelType::MONSTER_SHADOW_WRAITH: mesh = gen_shadow_wraith(p); break;
        case ModelType::MONSTER_TRIAL_SENTINEL: mesh = gen_trial_sentinel(p); break;
        case ModelType::MONSTER_SHADOW_KNIGHT: mesh = gen_shadow_knight(p); break;
        case ModelType::MONSTER_AIDA_GUARDIAN: mesh = gen_aida_guardian(p); break;
        case ModelType::MONSTER_WAVE_EMPEROR: mesh = gen_wave_emperor(p); break;
        case ModelType::MONSTER_BONE_DRAGON: mesh = gen_bone_dragon(p); break;
        case ModelType::MONSTER_CORRUPTED_SPRITE: mesh = gen_corrupted_sprite(p); break;
        case ModelType::MONSTER_SHADOW_WOLF: mesh = gen_shadow_wolf(p); break;
        case ModelType::MONSTER_CORRUPTED_TREANT: mesh = gen_corrupted_treant(p); break;
        case ModelType::MONSTER_BLUE_BRAIN: mesh = gen_blue_brain(p); break;
        case ModelType::MONSTER_SHADOW_GOLEM: mesh = gen_shadow_golem(p); break;
        case ModelType::MONSTER_CAVE_GUARDIAN: mesh = gen_cave_guardian(p); break;
        case ModelType::MONSTER_RANBABINGO: mesh = gen_ranbabingo(p); break;
        case ModelType::MONSTER_DNAVARATH: mesh = gen_dnavarath(p); break;
        
        // Items
        case ModelType::ITEM_POTION: mesh = gen_potion(p); break;
        case ModelType::ITEM_ETHER: mesh = gen_ether(p); break;
        case ModelType::ITEM_REVIVE: mesh = gen_revive(p); break;
        case ModelType::ITEM_ANTIDOTE: mesh = gen_antidote(p); break;
        case ModelType::ITEM_MAGIC_WATER: mesh = gen_magic_water(p); break;
        case ModelType::ITEM_ELIXIR: mesh = gen_elixir(p); break;
        case ModelType::ITEM_DATA_DRAIN: mesh = gen_data_drain(p); break;
        case ModelType::ITEM_SKILL_BOOK: mesh = gen_skill_book(p); break;
        case ModelType::ITEM_EQUIPMENT_BOX: mesh = gen_equipment_box(p); break;
        case ModelType::ITEM_CARMINA_GADELICA: mesh = gen_carmina_gadelica(p); break;
        case ModelType::ITEM_LIA_FAIL: mesh = gen_lia_fail(p); break;
        case ModelType::ITEM_KEY: mesh = gen_key(p); break;
        
        // Weapons
        case ModelType::WEAPON_DUAL_SWORDS: mesh = gen_dual_swords(p); break;
        case ModelType::WEAPON_HEAVY_BLADE: mesh = gen_heavy_blade(p); break;
        case ModelType::WEAPON_STAFF: mesh = gen_staff(p); break;
        case ModelType::WEAPON_SPEAR: mesh = gen_spear(p); break;
        case ModelType::WEAPON_DAGGER: mesh = gen_dagger(p); break;
        case ModelType::WEAPON_SHIELD: mesh = gen_shield(p); break;
        case ModelType::WEAPON_RYUKEN: mesh = gen_ryuken(p); break;
        case ModelType::WEAPON_AVATAR_KITE: mesh = gen_avatar_weapon_kite(p); break;
        
        // Avatars
        case ModelType::AVATAR_TSUKUYOMI: mesh = gen_avatar_tsukuyomi(p); break;
        case ModelType::AVATAR_KITE: mesh = gen_avatar_kite(p); break;
        case ModelType::AVATAR_HASEO_5TH: mesh = gen_avatar_haseo_5th(p); break;
        case ModelType::AVATAR_AZURE_KITE: mesh = gen_avatar_azure_kite(p); break;
        
        // Environment
        case ModelType::TREE_WILLOW: mesh = gen_tree_willow(p); break;
        case ModelType::TREE_OAK: mesh = gen_tree_oak(p); break;
        case ModelType::TREE_MAPLE: mesh = gen_tree_maple(p); break;
        case ModelType::TREE_BAMBOO: mesh = gen_tree_bamboo(p); break;
        case ModelType::TREE_CEDAR: mesh = gen_tree_cedar(p); break;
        case ModelType::TREE_PINE: mesh = gen_tree_pine(p); break;
        case ModelType::GRUNTY_MOUNT: mesh = gen_grunty(p); break;
        case ModelType::DATA_MOTE: mesh = gen_data_mote(p); break;
        case ModelType::SKY_GRID: mesh = gen_sky_grid(p); break;
        
        default: mesh = create_box_mesh(1, 1, 1); break;
    }
    
    if (mesh) {
        m_models[type] = mesh;
    }
    return mesh;
}

// ============================================================================
// STRUCTURE GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::gen_tower(const ModelParams& p) {
    return create_cylinder_mesh(p.width, p.width * 0.7f, p.height, 8);
}

WorldMesh* ModelGenerator::gen_pillar(const ModelParams& p) {
    return create_cylinder_mesh(p.width, p.width, p.height, 12);
}

WorldMesh* ModelGenerator::gen_monolith(const ModelParams& p) {
    auto mesh = create_box_mesh(p.width, p.height, p.depth);
    // Monoliths are tall dark slabs
    return mesh;
}

WorldMesh* ModelGenerator::gen_shrine(const ModelParams& p) {
    auto mesh = new WorldMesh();
    // Shrine: stepped pyramid base + crystal on top
    float base_w = p.width * 3;
    float base_h = p.height * 0.3f;
    float top_w = p.width;
    float top_h = p.height * 0.7f;
    
    // Base steps
    for (int step = 0; step < 3; step++) {
        float w = base_w - step * (base_w - top_w) / 3;
        float h = base_h / 3;
        float y = step * h;
        // Add step vertices...
    }
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::gen_gate(const ModelParams& p) {
    auto mesh = new WorldMesh();
    // Gate: two pillars + arch top
    float pillar_w = p.width * 0.3f;
    float pillar_h = p.height;
    float arch_w = p.width;
    float arch_h = p.height * 0.3f;
    
    // Left pillar
    // Right pillar
    // Arch
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::gen_market(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.width * 0.6f);
}

WorldMesh* ModelGenerator::gen_inn(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.width * 0.8f);
}

WorldMesh* ModelGenerator::gen_throne(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.width * 0.8f);
}

WorldMesh* ModelGenerator::gen_wall(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.depth * 5);
}

WorldMesh* ModelGenerator::gen_arch(const ModelParams& p) {
    return create_cylinder_mesh(p.width, p.width, p.height, 16);
}

WorldMesh* ModelGenerator::gen_bridge(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.depth * 10);
}

WorldMesh* ModelGenerator::gen_crystal(const ModelParams& p) {
    return create_crystal_mesh(p.width, p.height);
}

WorldMesh* ModelGenerator::gen_altar(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.width);
}

WorldMesh* ModelGenerator::gen_cave(const ModelParams& p) {
    return create_sphere_mesh(p.width, 12, 8);
}

WorldMesh* ModelGenerator::gen_spire(const ModelParams& p) {
    return create_cylinder_mesh(p.width * 0.3f, p.width * 0.1f, p.height, 6);
}

WorldMesh* ModelGenerator::gen_ruin(const ModelParams& p) {
    return create_box_mesh(p.width, p.height * 0.5f, p.depth);
}

// ============================================================================
// CHARACTER GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::gen_player_kite(const ModelParams& p) {
    auto mesh = create_character_mesh(p.height, p.width, p.depth, "R1");
    // Kite: blue/cyan color scheme, dual swords
    return mesh;
}

WorldMesh* ModelGenerator::gen_player_haseo(const ModelParams& p) {
    auto mesh = create_character_mesh(p.height, p.width, p.depth, "R2");
    // Haseo: black/red color scheme, ryuken weapon
    return mesh;
}

WorldMesh* ModelGenerator::gen_player_blackrose(const ModelParams& p) {
    auto mesh = create_character_mesh(p.height, p.width, p.depth, "R2");
    // BlackRose: green/nature theme, staff weapon
    return mesh;
}

WorldMesh* ModelGenerator::gen_player_balung(const ModelParams& p) {
    auto mesh = create_character_mesh(p.height, p.width, p.depth, "R1");
    // Balung: white/silver theme, dual swords
    return mesh;
}

// ============================================================================
// MONSTER GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::gen_skeleton(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "skeleton");
}

WorldMesh* ModelGenerator::gen_corrupted_guard(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "corrupted");
}

WorldMesh* ModelGenerator::gen_data_bug(const ModelParams& p) {
    return create_sphere_mesh(p.width, 8, 6);
}

WorldMesh* ModelGenerator::gen_cave_golem(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "golem");
}

WorldMesh* ModelGenerator::gen_shadow_wraith(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "shadow");
}

WorldMesh* ModelGenerator::gen_trial_sentinel(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "sentinel");
}

WorldMesh* ModelGenerator::gen_shadow_knight(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "shadow_knight");
}

WorldMesh* ModelGenerator::gen_aida_guardian(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "aida");
}

WorldMesh* ModelGenerator::gen_wave_emperor(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "boss");
}

WorldMesh* ModelGenerator::gen_bone_dragon(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "dragon");
}

WorldMesh* ModelGenerator::gen_corrupted_sprite(const ModelParams& p) {
    return create_sphere_mesh(p.width, 8, 6);
}

WorldMesh* ModelGenerator::gen_shadow_wolf(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "wolf");
}

WorldMesh* ModelGenerator::gen_corrupted_treant(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "treant");
}

WorldMesh* ModelGenerator::gen_blue_brain(const ModelParams& p) {
    return create_sphere_mesh(p.width, 12, 8);
}

WorldMesh* ModelGenerator::gen_shadow_golem(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "golem");
}

WorldMesh* ModelGenerator::gen_cave_guardian(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "guardian");
}

WorldMesh* ModelGenerator::gen_ranbabingo(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "beast");
}

WorldMesh* ModelGenerator::gen_dnavarath(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "beast");
}

// ============================================================================
// ITEM GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::gen_potion(const ModelParams& p) {
    return create_item_mesh("potion", p.height);
}

WorldMesh* ModelGenerator::gen_ether(const ModelParams& p) {
    return create_item_mesh("potion", p.height);
}

WorldMesh* ModelGenerator::gen_revive(const ModelParams& p) {
    return create_item_mesh("crystal", p.height);
}

WorldMesh* ModelGenerator::gen_antidote(const ModelParams& p) {
    return create_item_mesh("potion", p.height);
}

WorldMesh* ModelGenerator::gen_magic_water(const ModelParams& p) {
    return create_item_mesh("potion", p.height);
}

WorldMesh* ModelGenerator::gen_elixir(const ModelParams& p) {
    return create_item_mesh("crystal", p.height);
}

WorldMesh* ModelGenerator::gen_data_drain(const ModelParams& p) {
    return create_item_mesh("orb", p.height);
}

WorldMesh* ModelGenerator::gen_skill_book(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.depth);
}

WorldMesh* ModelGenerator::gen_equipment_box(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.depth);
}

WorldMesh* ModelGenerator::gen_carmina_gadelica(const ModelParams& p) {
    return create_weapon_mesh("staff", p.height);
}

WorldMesh* ModelGenerator::gen_lia_fail(const ModelParams& p) {
    return create_weapon_mesh("staff", p.height);
}

WorldMesh* ModelGenerator::gen_key(const ModelParams& p) {
    return create_item_mesh("key", p.height);
}

// ============================================================================
// WEAPON GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::gen_dual_swords(const ModelParams& p) {
    return create_weapon_mesh("sword", p.height);
}

WorldMesh* ModelGenerator::gen_heavy_blade(const ModelParams& p) {
    return create_weapon_mesh("greatsword", p.height);
}

WorldMesh* ModelGenerator::gen_staff(const ModelParams& p) {
    return create_weapon_mesh("staff", p.height);
}

WorldMesh* ModelGenerator::gen_spear(const ModelParams& p) {
    return create_weapon_mesh("spear", p.height);
}

WorldMesh* ModelGenerator::gen_dagger(const ModelParams& p) {
    return create_weapon_mesh("dagger", p.height);
}

WorldMesh* ModelGenerator::gen_shield(const ModelParams& p) {
    return create_box_mesh(p.width, p.height, p.depth);
}

WorldMesh* ModelGenerator::gen_ryuken(const ModelParams& p) {
    return create_weapon_mesh("sword", p.height);
}

WorldMesh* ModelGenerator::gen_avatar_weapon_kite(const ModelParams& p) {
    return create_weapon_mesh("avatar_sword", p.height);
}

// ============================================================================
// AVATAR GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::gen_avatar_tsukuyomi(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "avatar");
}

WorldMesh* ModelGenerator::gen_avatar_kite(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "avatar");
}

WorldMesh* ModelGenerator::gen_avatar_haseo_5th(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "avatar");
}

WorldMesh* ModelGenerator::gen_avatar_azure_kite(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "avatar");
}

// ============================================================================
// ENVIRONMENT GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::gen_tree_willow(const ModelParams& p) {
    return create_tree_mesh("willow", p.height);
}

WorldMesh* ModelGenerator::gen_tree_oak(const ModelParams& p) {
    return create_tree_mesh("oak", p.height);
}

WorldMesh* ModelGenerator::gen_tree_maple(const ModelParams& p) {
    return create_tree_mesh("maple", p.height);
}

WorldMesh* ModelGenerator::gen_tree_bamboo(const ModelParams& p) {
    return create_tree_mesh("bamboo", p.height);
}

WorldMesh* ModelGenerator::gen_tree_cedar(const ModelParams& p) {
    return create_tree_mesh("cedar", p.height);
}

WorldMesh* ModelGenerator::gen_tree_pine(const ModelParams& p) {
    return create_tree_mesh("pine", p.height);
}

WorldMesh* ModelGenerator::gen_grunty(const ModelParams& p) {
    return create_monster_mesh(p.height, p.width, p.depth, "grunty");
}

WorldMesh* ModelGenerator::gen_data_mote(const ModelParams& p) {
    return create_sphere_mesh(p.width, 6, 4);
}

WorldMesh* ModelGenerator::gen_sky_grid(const ModelParams& p) {
    return create_plane_mesh(p.width, p.depth, 20);
}

// ============================================================================
// HELPER MESH GENERATORS
// ============================================================================

WorldMesh* ModelGenerator::create_box_mesh(float w, float h, float d) {
    auto mesh = new WorldMesh();
    float hw = w / 2, hh = h / 2, hd = d / 2;
    
    // 8 corners
    WorldVertex v;
    v.nx = 0; v.ny = 0; v.nz = 1;
    
    // Front face
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? hw : -hw;
        v.y = (i == 2 || i == 3) ? hh : -hh;
        v.z = hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Back face
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? -hw : hw;
        v.y = (i == 2 || i == 3) ? hh : -hh;
        v.z = -hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Top face
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? hw : -hw;
        v.y = hh;
        v.z = (i == 2 || i == 3) ? -hd : hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Bottom face
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? hw : -hw;
        v.y = -hh;
        v.z = (i == 2 || i == 3) ? hd : -hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Left face
    for (int i = 0; i < 4; i++) {
        v.x = -hw;
        v.y = (i == 2 || i == 3) ? hh : -hh;
        v.z = (i == 1 || i == 2) ? hd : -hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Right face
    for (int i = 0; i < 4; i++) {
        v.x = hw;
        v.y = (i == 2 || i == 3) ? hh : -hh;
        v.z = (i == 1 || i == 2) ? -hd : hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Indices for all 6 faces
    for (int face = 0; face < 6; face++) {
        int base = face * 4;
        mesh->indices.push_back(base);
        mesh->indices.push_back(base + 1);
        mesh->indices.push_back(base + 2);
        mesh->indices.push_back(base);
        mesh->indices.push_back(base + 2);
        mesh->indices.push_back(base + 3);
    }
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::create_sphere_mesh(float radius, int segments, int rings) {
    auto mesh = new WorldMesh();
    
    for (int r = 0; r <= rings; r++) {
        float phi = (float)r / rings * M_PI;
        for (int s = 0; s <= segments; s++) {
            float theta = (float)s / segments * 2 * M_PI;
            
            WorldVertex v;
            v.x = radius * sin(phi) * cos(theta);
            v.y = radius * cos(phi);
            v.z = radius * sin(phi) * sin(theta);
            v.nx = sin(phi) * cos(theta);
            v.ny = cos(phi);
            v.nz = sin(phi) * sin(theta);
            v.u = (float)s / segments;
            v.v = (float)r / rings;
            mesh->vertices.push_back(v);
        }
    }
    
    for (int r = 0; r < rings; r++) {
        for (int s = 0; s < segments; s++) {
            int i0 = r * (segments + 1) + s;
            int i1 = i0 + 1;
            int i2 = i0 + (segments + 1);
            int i3 = i2 + 1;
            mesh->indices.push_back(i0);
            mesh->indices.push_back(i2);
            mesh->indices.push_back(i1);
            mesh->indices.push_back(i1);
            mesh->indices.push_back(i2);
            mesh->indices.push_back(i3);
        }
    }
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::create_cylinder_mesh(float r1, float r2, float h, int segments) {
    auto mesh = new WorldMesh();
    float hh = h / 2;
    
    for (int i = 0; i <= segments; i++) {
        float a = (float)i / segments * 2 * M_PI;
        float cos_a = cos(a);
        float sin_a = sin(a);
        
        // Bottom
        WorldVertex vb;
        vb.x = cos_a * r1;
        vb.y = -hh;
        vb.z = sin_a * r1;
        vb.nx = cos_a; vb.ny = 0; vb.nz = sin_a;
        vb.u = (float)i / segments;
        vb.v = 0;
        mesh->vertices.push_back(vb);
        
        // Top
        WorldVertex vt;
        vt.x = cos_a * r2;
        vt.y = hh;
        vt.z = sin_a * r2;
        vt.nx = cos_a; vt.ny = 0; vt.nz = sin_a;
        vt.u = (float)i / segments;
        vt.v = 1;
        mesh->vertices.push_back(vt);
    }
    
    for (int i = 0; i < segments; i++) {
        int base = i * 2;
        mesh->indices.push_back(base);
        mesh->indices.push_back(base + 1);
        mesh->indices.push_back(base + 2);
        mesh->indices.push_back(base + 1);
        mesh->indices.push_back(base + 3);
        mesh->indices.push_back(base + 2);
    }
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::create_plane_mesh(float w, float d, int subdiv) {
    auto mesh = new WorldMesh();
    float step_w = w / subdiv;
    float step_d = d / subdiv;
    float start_w = -w / 2;
    float start_d = -d / 2;
    
    for (int z = 0; z <= subdiv; z++) {
        for (int x = 0; x <= subdiv; x++) {
            WorldVertex v;
            v.x = start_w + x * step_w;
            v.y = 0;
            v.z = start_d + z * step_d;
            v.nx = 0; v.ny = 1; v.nz = 0;
            v.u = (float)x / subdiv;
            v.v = (float)z / subdiv;
            mesh->vertices.push_back(v);
        }
    }
    
    for (int z = 0; z < subdiv; z++) {
        for (int x = 0; x < subdiv; x++) {
            int i0 = z * (subdiv + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (subdiv + 1);
            int i3 = i2 + 1;
            mesh->indices.push_back(i0);
            mesh->indices.push_back(i2);
            mesh->indices.push_back(i1);
            mesh->indices.push_back(i1);
            mesh->indices.push_back(i2);
            mesh->indices.push_back(i3);
        }
    }
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::create_crystal_mesh(float r, float h) {
    auto mesh = new WorldMesh();
    float hh = h / 2;
    
    // Crystal: diamond shape (octahedron)
    WorldVertex v;
    v.nx = 0; v.ny = 1; v.nz = 0;
    
    // Top point
    v.x = 0; v.y = hh; v.z = 0; v.u = 0.5f; v.v = 1.0f;
    mesh->vertices.push_back(v);
    
    // Middle ring
    for (int i = 0; i < 4; i++) {
        float a = (float)i / 4 * 2 * M_PI;
        v.x = cos(a) * r;
        v.y = 0;
        v.z = sin(a) * r;
        v.u = (float)i / 4;
        v.v = 0.5f;
        mesh->vertices.push_back(v);
    }
    
    // Bottom point
    v.x = 0; v.y = -hh; v.z = 0; v.u = 0.5f; v.v = 0.0f;
    mesh->vertices.push_back(v);
    
    // Top faces
    for (int i = 0; i < 4; i++) {
        mesh->indices.push_back(0);
        mesh->indices.push_back(i + 1);
        mesh->indices.push_back((i + 1) % 4 + 1);
    }
    
    // Bottom faces
    for (int i = 0; i < 4; i++) {
        mesh->indices.push_back(5);
        mesh->indices.push_back((i + 1) % 4 + 1);
        mesh->indices.push_back(i + 1);
    }
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::create_character_mesh(float height, float width, float depth, const std::string& era) {
    auto mesh = new WorldMesh();
    float hh = height / 2;
    float hw = width / 2;
    float hd = depth / 2;
    
    // Body (torso)
    WorldVertex v;
    v.nx = 0; v.ny = 0; v.nz = 1;
    
    // Front torso
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? hw : -hw;
        v.y = (i == 2 || i == 3) ? hh * 0.7f : -hh * 0.3f;
        v.z = hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Head (sphere approximation)
    float head_r = width * 0.4f;
    float head_y = hh * 0.7f + head_r;
    
    // Simple head box
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? head_r : -head_r;
        v.y = (i == 2 || i == 3) ? head_y + head_r : head_y - head_r;
        v.z = head_r * 0.5f;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Arms
    for (int side = 0; side < 2; side++) {
        float arm_x = (side == 0) ? -hw * 1.2f : hw * 1.2f;
        for (int i = 0; i < 4; i++) {
            v.x = arm_x + ((i == 1 || i == 2) ? hw * 0.2f : -hw * 0.2f);
            v.y = (i == 2 || i == 3) ? hh * 0.5f : -hh * 0.2f;
            v.z = hd * 0.5f;
            v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
            v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
            mesh->vertices.push_back(v);
        }
    }
    
    // Legs
    for (int side = 0; side < 2; side++) {
        float leg_x = (side == 0) ? -hw * 0.4f : hw * 0.4f;
        for (int i = 0; i < 4; i++) {
            v.x = leg_x + ((i == 1 || i == 2) ? hw * 0.25f : -hw * 0.25f);
            v.y = (i == 2 || i == 3) ? -hh * 0.3f : -hh;
            v.z = hd * 0.5f;
            v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
            v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
            mesh->vertices.push_back(v);
        }
    }
    
    // Indices for all parts
    int parts = 5; // torso, head, 2 arms, 2 legs
    for (int part = 0; part < parts; part++) {
        int base = part * 4;
        mesh->indices.push_back(base);
        mesh->indices.push_back(base + 1);
        mesh->indices.push_back(base + 2);
        mesh->indices.push_back(base);
        mesh->indices.push_back(base + 2);
        mesh->indices.push_back(base + 3);
    }
    
    mesh->upload();
    return mesh;
}

WorldMesh* ModelGenerator::create_monster_mesh(float height, float width, float depth, const std::string& type) {
    // Monsters are similar to characters but more menacing
    return create_character_mesh(height, width, depth, "monster");
}

WorldMesh* ModelGenerator::create_weapon_mesh(const std::string& type, float length) {
    auto mesh = new WorldMesh();
    
    if (type == "sword" || type == "greatsword") {
        // Blade
        float blade_w = length * 0.1f;
        float blade_h = length * 0.8f;
        float hilt_h = length * 0.2f;
        
        // Blade (thin box)
        WorldVertex v;
        v.nx = 0; v.ny = 0; v.nz = 1;
        for (int i = 0; i < 4; i++) {
            v.x = (i == 1 || i == 2) ? blade_w / 2 : -blade_w / 2;
            v.y = (i == 2 || i == 3) ? blade_h : 0;
            v.z = 0.01f;
            v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
            v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
            mesh->vertices.push_back(v);
        }
        
        // Hilt
        for (int i = 0; i < 4; i++) {
            v.x = (i == 1 || i == 2) ? blade_w : -blade_w;
            v.y = (i == 2 || i == 3) ? 0 : -hilt_h;
            v.z = 0.01f;
            v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
            v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
            mesh->vertices.push_back(v);
        }
        
        // Indices
        mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
        mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(3);
        mesh->indices.push_back(4); mesh->indices.push_back(5); mesh->indices.push_back(6);
        mesh->indices.push_back(4); mesh->indices.push_back(6); mesh->indices.push_back(7);
    }
    else if (type == "staff") {
        mesh = create_cylinder_mesh(0.05f, 0.05f, length, 8);
    }
    else if (type == "spear") {
        mesh = create_cylinder_mesh(0.03f, 0.01f, length, 8);
    }
    else if (type == "dagger") {
        mesh = create_weapon_mesh("sword", length);
    }
    else {
        mesh = create_box_mesh(0.1f, length, 0.05f);
    }
    
    return mesh;
}

WorldMesh* ModelGenerator::create_item_mesh(const std::string& type, float size) {
    if (type == "potion") {
        return create_cylinder_mesh(size * 0.3f, size * 0.4f, size, 8);
    }
    else if (type == "crystal") {
        return create_crystal_mesh(size * 0.5f, size);
    }
    else if (type == "orb") {
        return create_sphere_mesh(size * 0.5f, 8, 6);
    }
    else if (type == "key") {
        return create_box_mesh(size * 0.3f, size, size * 0.1f);
    }
    return create_box_mesh(size, size, size);
}

WorldMesh* ModelGenerator::create_tree_mesh(const std::string& type, float height) {
    auto mesh = new WorldMesh();
    
    // Trunk
    float trunk_h = height * 0.4f;
    float trunk_r = height * 0.05f;
    
    // Canopy (different shapes per tree type)
    float canopy_h = height * 0.6f;
    float canopy_r = height * 0.3f;
    
    if (type == "bamboo") {
        canopy_r = height * 0.05f;
        canopy_h = height * 0.6f;
    }
    else if (type == "pine" || type == "cedar") {
        // Cone shape
        canopy_h = height * 0.7f;
    }
    
    // Generate trunk + canopy
    WorldVertex v;
    v.nx = 0; v.ny = 0; v.nz = 1;
    
    // Trunk vertices
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? trunk_r : -trunk_r;
        v.y = (i == 2 || i == 3) ? trunk_h : 0;
        v.z = trunk_r;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Canopy vertices
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? canopy_r : -canopy_r;
        v.y = (i == 2 || i == 3) ? trunk_h + canopy_h : trunk_h;
        v.z = canopy_r;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Indices
    mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
    mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(3);
    mesh->indices.push_back(4); mesh->indices.push_back(5); mesh->indices.push_back(6);
    mesh->indices.push_back(4); mesh->indices.push_back(6); mesh->indices.push_back(7);
    
    mesh->upload();
    return mesh;
}

// ============================================================================
// DISC/ZONE MODEL GENERATION
// ============================================================================

std::vector<WorldMesh*> ModelGenerator::generate_disc_models(const std::string& disc_id) {
    std::vector<WorldMesh*> models;
    
    // Generate all models needed for this disc
    // This is a simplified version - in production, you'd read from world definitions
    
    // Always generate these core models
    models.push_back(generate_model(ModelType::TOWER));
    models.push_back(generate_model(ModelType::PILLAR));
    models.push_back(generate_model(ModelType::MONOLITH));
    models.push_back(generate_model(ModelType::SHRINE));
    models.push_back(generate_model(ModelType::GATE));
    models.push_back(generate_model(ModelType::CRYSTAL));
    models.push_back(generate_model(ModelType::ALTAR));
    
    // Generate disc-specific models
    if (disc_id == "SIGN" || disc_id == "INFECTION" || disc_id == "FREQUENCY" || 
        disc_id == "OUTBREAK" || disc_id == "QUARANTINE") {
        // R1 era models
        models.push_back(generate_model(ModelType::PLAYER_KITE));
        models.push_back(generate_model(ModelType::PLAYER_BALMUNG));
        models.push_back(generate_model(ModelType::WEAPON_DUAL_SWORDS));
        models.push_back(generate_model(ModelType::AVATAR_TSUKUYOMI));
        models.push_back(generate_model(ModelType::AVATAR_KITE));
    }
    else if (disc_id == "GU" || disc_id.find("vol") != std::string::npos) {
        // R2 era models
        models.push_back(generate_model(ModelType::PLAYER_HASEO));
        models.push_back(generate_model(ModelType::PLAYER_BLACKROSE));
        models.push_back(generate_model(ModelType::WEAPON_RYUKEN));
        models.push_back(generate_model(ModelType::WEAPON_HEAVY_BLADE));
        models.push_back(generate_model(ModelType::AVATAR_HASEO_5TH));
        models.push_back(generate_model(ModelType::AVATAR_AZURE_KITE));
    }
    else if (disc_id == "LINK") {
        // Link era models
        models.push_back(generate_model(ModelType::PLAYER_KITE));
        models.push_back(generate_model(ModelType::WEAPON_DUAL_SWORDS));
    }
    
    // Common monsters
    models.push_back(generate_model(ModelType::MONSTER_SKELETON));
    models.push_back(generate_model(ModelType::MONSTER_CORRUPTED_GUARD));
    models.push_back(generate_model(ModelType::MONSTER_DATA_BUG));
    models.push_back(generate_model(ModelType::MONSTER_CAVE_GOLEM));
    models.push_back(generate_model(ModelType::MONSTER_SHADOW_WRAITH));
    models.push_back(generate_model(ModelType::MONSTER_SHADOW_KNIGHT));
    models.push_back(generate_model(ModelType::MONSTER_AIDA_GUARDIAN));
    
    // Common items
    models.push_back(generate_model(ModelType::ITEM_POTION));
    models.push_back(generate_model(ModelType::ITEM_ETHER));
    models.push_back(generate_model(ModelType::ITEM_REVIVE));
    models.push_back(generate_model(ModelType::ITEM_ELIXIR));
    models.push_back(generate_model(ModelType::ITEM_DATA_DRAIN));
    models.push_back(generate_model(ModelType::ITEM_SKILL_BOOK));
    
    // Common environment
    models.push_back(generate_model(ModelType::TREE_OAK));
    models.push_back(generate_model(ModelType::TREE_PINE));
    models.push_back(generate_model(ModelType::TREE_BAMBOO));
    models.push_back(generate_model(ModelType::DATA_MOTE));
    models.push_back(generate_model(ModelType::SKY_GRID));
    
    return models;
}

std::vector<WorldMesh*> ModelGenerator::generate_zone_models(const std::string& zone_name) {
    std::vector<WorldMesh*> models;
    
    // Generate models specific to this zone type
    if (zone_name == "Mac Anu" || zone_name == "Leiseijo") {
        // Root town
        models.push_back(generate_model(ModelType::TOWER));
        models.push_back(generate_model(ModelType::MARKET));
        models.push_back(generate_model(ModelType::INN));
        models.push_back(generate_model(ModelType::GATE));
    }
    else if (zone_name == "Dun Loireag" || zone_name == "Cave of Trial") {
        // Dungeon
        models.push_back(generate_model(ModelType::PILLAR));
        models.push_back(generate_model(ModelType::CRYSTAL));
        models.push_back(generate_model(ModelType::ALTAR));
        models.push_back(generate_model(ModelType::CAVE));
    }
    else if (zone_name == "Twilight Palace") {
        // Palace
        models.push_back(generate_model(ModelType::THRONE));
        models.push_back(generate_model(ModelType::SPIRE));
        models.push_back(generate_model(ModelType::ARCH));
        models.push_back(generate_model(ModelType::WALL));
    }
    else if (zone_name == "Dragonbone Wastes") {
        // Wastes
        models.push_back(generate_model(ModelType::MONOLITH));
        models.push_back(generate_model(ModelType::RUIN));
        models.push_back(generate_model(ModelType::MONSTER_BONE_DRAGON));
    }
    else if (zone_name == "Kughai Valley" || zone_name == "Mystic Valley") {
        // Valley
        models.push_back(generate_model(ModelType::TREE_WILLOW));
        models.push_back(generate_model(ModelType::TREE_MAPLE));
        models.push_back(generate_model(ModelType::BRIDGE));
        models.push_back(generate_model(ModelType::SHRINE));
    }
    else {
        // Default field
        models.push_back(generate_model(ModelType::TREE_OAK));
        models.push_back(generate_model(ModelType::TREE_PINE));
        models.push_back(generate_model(ModelType::SHRINE));
        models.push_back(generate_model(ModelType::GATE));
    }
    
    return models;
}

} // namespace te
