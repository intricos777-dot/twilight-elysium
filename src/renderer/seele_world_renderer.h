#pragma once
#include "renderer/sdl2_renderer.h"
#include "ai/seele_ai.h"
#include "ai/hackgu_content.h"
#include "engine/math_types.h"
#include <vector>
#include <string>
#include <memory>

namespace te {

// ============================================================================
// SEELE WORLD RENDERER - 3D world using original game assets + AI generation
// ============================================================================

struct WorldVertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

struct WorldMesh {
    std::vector<WorldVertex> vertices;
    std::vector<uint32_t> indices;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    GLuint texture_id = 0;
    
    void upload();
    void draw();
    void cleanup();
};

struct WorldObject {
    std::string name;
    std::string type;  // "structure", "npc", "monster", "prop"
    float x, y, z;
    float scale;
    WorldMesh* mesh;
    seele::ProceduralAsset* texture;
};

class SeeleWorldRenderer {
public:
    SeeleWorldRenderer();
    ~SeeleWorldRenderer();

    bool initialize(SDL2Renderer* renderer, seele::SeeleAIModule* ai);
    void shutdown();

    // Load original game assets
    bool load_original_assets(const std::string& asset_dir);
    
    // Generate world from Seele AI
    void generate_world(const std::string& disc_id, const std::string& zone_name);
    
    // Load zone data from JSON
    bool load_zone_data(const std::string& json_path, const std::string& disc_id);
    
    // Render frame
    void render(float dt);
    
    // Camera control
    void rotate_camera(float dx, float dy);
    void move_camera(float dx, float dz);
    void zoom_camera(float delta);

    // Access
    SDL2Renderer* renderer() { return m_renderer; }
    seele::SeeleAIModule* ai_module() { return m_ai; }
    
    // Stats
    int object_count() const { return (int)m_objects.size(); }
    int mesh_count() const { return (int)m_meshes.size(); }
    int texture_count() const { return (int)m_textures.size(); }

private:
    SDL2Renderer* m_renderer = nullptr;
    seele::SeeleAIModule* m_ai = nullptr;
    hackgu::HackguContentGenerator* m_content_gen = nullptr;
    
    std::vector<WorldMesh*> m_meshes;
    std::vector<WorldObject*> m_objects;
    std::vector<seele::ProceduralAsset*> m_textures;
    std::vector<GLShader*> m_shaders;
    
    // Camera
    float m_cam_x = 0.0f, m_cam_y = 5.0f, m_cam_z = 15.0f;
    mtx m_view;
    mtx m_proj;
    float m_rot_x = 0.0f, m_rot_y = 0.0f;
    float m_zoom = 1.0f;
    
    // World data
    struct Zone {
        std::string id;
        std::string name;
        std::string type;
        int sky[3];
        int ground[3];
        int fog;
        std::vector<std::string> structures;
        std::vector<std::string> npcs;
        std::vector<std::string> monsters;
    };
    std::vector<Zone> m_zones;
    int m_current_zone = 0;
    
    // Mesh generation
    WorldMesh* create_plane(float w, float d, int subdiv);
    WorldMesh* create_box(float w, float h, float d);
    WorldMesh* create_terrain(float size, int res, float height);
    WorldMesh* create_tower(float r1, float r2, float h);
    WorldMesh* create_pillar(float r, float h);
    WorldMesh* create_character(float h);
    
    // Texture generation
    void generate_zone_textures(const Zone& zone);
    void generate_structure_textures(const Zone& zone);
    void generate_character_textures(const Zone& zone);
    
    // Object placement
    void place_structures(const Zone& zone);
    void place_npcs(const Zone& zone);
    void place_monsters(const Zone& zone);
    void place_terrain(const Zone& zone);
    
    // Shader
    GLShader* m_world_shader = nullptr;
    bool load_shaders();
    
    // Matrices
    void update_view_matrix();
    void update_proj_matrix(float fov, float aspect);
};

} // namespace te
