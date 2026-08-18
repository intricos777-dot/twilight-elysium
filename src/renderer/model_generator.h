#pragma once
#include "seele_world_renderer.h"
#include <vector>
#include <string>
#include <map>

namespace te {

// ============================================================================
// 3D MODEL GENERATION SYSTEM
// ============================================================================

// Model types for .hack world
enum class ModelType {
    // Structures
    TOWER, PILLAR, MONOLITH, SHRINE, GATE, MARKET, INN, THRONE, WALL, ARCH,
    BRIDGE, CRYSTAL, ALTAR, CAVE, SPIRE, RUIN,
    // Characters  
    PLAYER_KITE, PLAYER_HASEO, PLAYER_BLACKROSE, PLAYER_BALMUNG,
    NPC_MIMIRU, NPC_BEAR, NPC_BT, NPC_WISEMAN, NPC_SUBARU, NPC_TSUKASA,
    NPC_OVAN, NPC_ATOLI, NPC_PI, NPC_KREWHNA, NPC_TRIEDGE,
    NPC_SAKUYA, NPC_TOBIAS, NPC_SHINO, NPC_ALKAID, NPC_MISTRAL, NPC_ROSE,
    NPC_ZELKII, NPC_PHOENIX, NPC_AURA, NPC_ZEFIE, NPC_MARIEL,
    // Monsters
    MONSTER_SKELETON, MONSTER_CORRUPTED_GUARD, MONSTER_DATA_BUG,
    MONSTER_CAVE_GOLEM, MONSTER_SHADOW_WRAITH, MONSTER_TRIAL_SENTINEL,
    MONSTER_SHADOW_KNIGHT, MONSTER_AIDA_GUARDIAN, MONSTER_WAVE_EMPEROR,
    MONSTER_BONE_DRAGON, MONSTER_CORRUPTED_SPRITE, MONSTER_SHADOW_WOLF,
    MONSTER_CORRUPTED_TREANT, MONSTER_BLUE_BRAIN, MONSTER_SHADOW_GOLEM,
    MONSTER_CAVE_GUARDIAN, MONSTER_RANBABINGO, MONSTER_DNAVARATH,
    // Items
    ITEM_POTION, ITEM_ETHER, ITEM_REVIVE, ITEM_ANTIDOTE, ITEM_MAGIC_WATER,
    ITEM_ELIXIR, ITEM_DATA_DRAIN, ITEM_SKILL_BOOK, ITEM_EQUIPMENT_BOX,
    ITEM_CARMINA_GADELICA, ITEM_LIA_FAIL, ITEM_KEY,
    // Weapons
    WEAPON_DUAL_SWORDS, WEAPON_HEAVY_BLADE, WEAPON_STAFF, WEAPON_SPEAR,
    WEAPON_DAGGER, WEAPON_SHIELD, WEAPON_RYUKEN, WEAPON_AVATAR_KITE,
    // Avatars
    AVATAR_TSUKUYOMI, AVATAR_KITE, AVATAR_HASEO_5TH, AVATAR_AZURE_KITE,
    // Environment
    TREE_WILLOW, TREE_OAK, TREE_MAPLE, TREE_BAMBOO, TREE_CEDAR, TREE_PINE,
    GRUNTY_MOUNT, DATA_MOTE, SKY_GRID
};

// Model generation parameters
struct ModelParams {
    float scale = 1.0f;
    float height = 1.8f;
    float width = 0.5f;
    float depth = 0.3f;
    int segments = 16;
    int subdivisions = 8;
    float complexity = 1.0f;
    std::string era = "R1";  // R1, R2, Link
    std::string character_class = "player";  // player, npc, monster
    bool has_animations = false;
    bool has_avatar_form = false;
};

class ModelGenerator {
public:
    ModelGenerator();
    ~ModelGenerator();

    // Initialize with Seele AI for procedural generation
    void initialize(seele::SeeleAIModule* ai);

    // Generate a model by type
    WorldMesh* generate_model(ModelType type, const ModelParams& params = {});
    
    // Generate all models for a disc
    std::vector<WorldMesh*> generate_disc_models(const std::string& disc_id);
    
    // Generate all models for a zone
    std::vector<WorldMesh*> generate_zone_models(const std::string& zone_name);
    
    // Get model info
    std::string model_name(ModelType type);
    ModelParams default_params(ModelType type);
    
    // Stats
    int generated_count() const { return (int)m_models.size(); }

private:
    seele::SeeleAIModule* m_ai = nullptr;
    std::map<ModelType, WorldMesh*> m_models;
    
    // Structure generators
    WorldMesh* gen_tower(const ModelParams& p);
    WorldMesh* gen_pillar(const ModelParams& p);
    WorldMesh* gen_monolith(const ModelParams& p);
    WorldMesh* gen_shrine(const ModelParams& p);
    WorldMesh* gen_gate(const ModelParams& p);
    WorldMesh* gen_market(const ModelParams& p);
    WorldMesh* gen_inn(const ModelParams& p);
    WorldMesh* gen_throne(const ModelParams& p);
    WorldMesh* gen_wall(const ModelParams& p);
    WorldMesh* gen_arch(const ModelParams& p);
    WorldMesh* gen_bridge(const ModelParams& p);
    WorldMesh* gen_crystal(const ModelParams& p);
    WorldMesh* gen_altar(const ModelParams& p);
    WorldMesh* gen_cave(const ModelParams& p);
    WorldMesh* gen_spire(const ModelParams& p);
    WorldMesh* gen_ruin(const ModelParams& p);
    
    // Character generators
    WorldMesh* gen_player_kite(const ModelParams& p);
    WorldMesh* gen_player_haseo(const ModelParams& p);
    WorldMesh* gen_player_blackrose(const ModelParams& p);
    WorldMesh* gen_player_balung(const ModelParams& p);
    WorldMesh* gen_npc_generic(const ModelParams& p, const std::string& name);
    WorldMesh* gen_avatar_tsukuyomi(const ModelParams& p);
    WorldMesh* gen_avatar_kite(const ModelParams& p);
    WorldMesh* gen_avatar_haseo_5th(const ModelParams& p);
    WorldMesh* gen_avatar_azure_kite(const ModelParams& p);
    
    // Monster generators
    WorldMesh* gen_skeleton(const ModelParams& p);
    WorldMesh* gen_corrupted_guard(const ModelParams& p);
    WorldMesh* gen_data_bug(const ModelParams& p);
    WorldMesh* gen_cave_golem(const ModelParams& p);
    WorldMesh* gen_shadow_wraith(const ModelParams& p);
    WorldMesh* gen_trial_sentinel(const ModelParams& p);
    WorldMesh* gen_shadow_knight(const ModelParams& p);
    WorldMesh* gen_aida_guardian(const ModelParams& p);
    WorldMesh* gen_wave_emperor(const ModelParams& p);
    WorldMesh* gen_bone_dragon(const ModelParams& p);
    WorldMesh* gen_corrupted_sprite(const ModelParams& p);
    WorldMesh* gen_shadow_wolf(const ModelParams& p);
    WorldMesh* gen_corrupted_treant(const ModelParams& p);
    WorldMesh* gen_blue_brain(const ModelParams& p);
    WorldMesh* gen_shadow_golem(const ModelParams& p);
    WorldMesh* gen_cave_guardian(const ModelParams& p);
    WorldMesh* gen_ranbabingo(const ModelParams& p);
    WorldMesh* gen_dnavarath(const ModelParams& p);
    
    // Item generators
    WorldMesh* gen_potion(const ModelParams& p);
    WorldMesh* gen_ether(const ModelParams& p);
    WorldMesh* gen_revive(const ModelParams& p);
    WorldMesh* gen_antidote(const ModelParams& p);
    WorldMesh* gen_magic_water(const ModelParams& p);
    WorldMesh* gen_elixir(const ModelParams& p);
    WorldMesh* gen_data_drain(const ModelParams& p);
    WorldMesh* gen_skill_book(const ModelParams& p);
    WorldMesh* gen_equipment_box(const ModelParams& p);
    WorldMesh* gen_carmina_gadelica(const ModelParams& p);
    WorldMesh* gen_lia_fail(const ModelParams& p);
    WorldMesh* gen_key(const ModelParams& p);
    
    // Weapon generators
    WorldMesh* gen_dual_swords(const ModelParams& p);
    WorldMesh* gen_heavy_blade(const ModelParams& p);
    WorldMesh* gen_staff(const ModelParams& p);
    WorldMesh* gen_spear(const ModelParams& p);
    WorldMesh* gen_dagger(const ModelParams& p);
    WorldMesh* gen_shield(const ModelParams& p);
    WorldMesh* gen_ryuken(const ModelParams& p);
    WorldMesh* gen_avatar_weapon_kite(const ModelParams& p);
    
    // Environment generators
    WorldMesh* gen_tree_willow(const ModelParams& p);
    WorldMesh* gen_tree_oak(const ModelParams& p);
    WorldMesh* gen_tree_maple(const ModelParams& p);
    WorldMesh* gen_tree_bamboo(const ModelParams& p);
    WorldMesh* gen_tree_cedar(const ModelParams& p);
    WorldMesh* gen_tree_pine(const ModelParams& p);
    WorldMesh* gen_grunty(const ModelParams& p);
    WorldMesh* gen_data_mote(const ModelParams& p);
    WorldMesh* gen_sky_grid(const ModelParams& p);
    
    // Helper: create a box with given dimensions
    WorldMesh* create_box_mesh(float w, float h, float d);
    // Helper: create a sphere
    WorldMesh* create_sphere_mesh(float radius, int segments, int rings);
    // Helper: create a cylinder
    WorldMesh* create_cylinder_mesh(float r1, float r2, float h, int segments);
    // Helper: create a plane
    WorldMesh* create_plane_mesh(float w, float d, int subdiv);
    // Helper: create a crystal (diamond shape)
    WorldMesh* create_crystal_mesh(float r, float h);
    // Helper: create a character body
    WorldMesh* create_character_mesh(float height, float width, float depth, 
                                      const std::string& era);
    // Helper: create a monster body
    WorldMesh* create_monster_mesh(float height, float width, float depth,
                                    const std::string& type);
    // Helper: create a weapon mesh
    WorldMesh* create_weapon_mesh(const std::string& type, float length);
    // Helper: create an item mesh
    WorldMesh* create_item_mesh(const std::string& type, float size);
    // Helper: create a tree mesh
    WorldMesh* create_tree_mesh(const std::string& type, float height);
};

} // namespace te
