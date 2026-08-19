#pragma once
#include "renderer.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>

namespace te {

// ============================================================
// PBR MATERIAL SYSTEM
// ============================================================

struct PBRMaterial {
    std::string name;
    
    // Textures
    GLuint albedo_map = 0;
    GLuint normal_map = 0;
    GLuint metallic_roughness_map = 0;
    GLuint ao_map = 0;
    GLuint emissive_map = 0;
    
    // Constants
    float albedo[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    float metallic = 0.0f;
    float roughness = 0.5f;
    float ao = 1.0f;
    float emissive[3] = {0.0f, 0.0f, 0.0f};
    
    // .hack specific
    float glow_intensity = 0.0f;
    float scanline_intensity = 0.3f;
    float fog_density = 0.5f;
    
    void bind(GLuint shader);
    void unbind();
};

// ============================================================
// PBR SHADER
// ============================================================

class PBRShader {
public:
    PBRShader() = default;
    ~PBRShader();
    
    bool load_from_files(const std::string& vert_path, const std::string& frag_path);
    bool create_from_source(const std::string& vert_src, const std::string& frag_src);
    
    GLuint program() const { return m_program; }
    void use();
    
    // Uniform setters
    void set_mat4(const std::string& name, const float* mat);
    void set_vec3(const std::string& name, float x, float y, float z);
    void set_vec4(const std::string& name, float x, float y, float z, float w);
    void set_float(const std::string& name, float val);
    void set_int(const std::string& name, int val);
    void set_texture(const std::string& name, GLuint texture, int unit);
    
private:
    GLuint m_program = 0;
    static GLuint compile_shader(GLenum type, const std::string& source);
};

// ============================================================
// MESH
// ============================================================

struct Vertex {
    float position[3];
    float normal[3];
    float uv[2];
    float tangent[4];
};

struct Mesh {
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ibo = 0;
    
    void upload();
    void draw();
    void cleanup();
};

// ============================================================
// MODEL
// ============================================================

struct Model {
    std::string name;
    std::vector<Mesh*> meshes;
    std::vector<PBRMaterial*> materials;
    float transform[16];
    
    void draw(PBRShader* shader);
};

// ============================================================
// SEELE AI ASSET PIPELINE
// ============================================================

class SeeleAssetGenerator {
public:
    SeeleAssetGenerator(SeeleAIModule* ai);
    
    // Generate a complete model from a description
    Model* generate_model(const std::string& description, const std::string& style);
    
    // Generate PBR material from description
    PBRMaterial* generate_material(const std::string& description, const std::string& base_color);
    
    // Generate shader for a zone type
    PBRShader* generate_shader(const std::string& zone_type);
    
    // Generate texture from description (using Seele AI)
    GLuint generate_texture(const std::string& description, int width, int height);
    
    // Generate normal map from height
    GLuint generate_normal_map(GLuint height_texture, int width, int height);
    
    // Load original game texture
    GLuint load_original_texture(const std::string& path);
    
private:
    SeeleAIModule* m_ai;
    std::map<std::string, Model*> m_model_cache;
    std::map<std::string, PBRMaterial*> m_material_cache;
    std::map<std::string, PBRShader*> m_shader_cache;
};

// ============================================================
// POST-PROCESSING
// ============================================================

class PostProcessor {
public:
    PostProcessor();
    ~PostProcessor();
    
    bool initialize(int width, int height);
    void shutdown();
    
    // Begin rendering to framebuffer
    void begin_scene();
    
    // End scene and render to screen with post-processing
    void end_scene(PBRShader* shader);
    
    // Effects
    void set_bloom_intensity(float v) { m_bloom_intensity = v; }
    void set_scanline_intensity(float v) { m_scanline_intensity = v; }
    void set_vignette_intensity(float v) { m_vignette_intensity = v; }
    void set_chromatic_aberration(float v) { m_chromatic = v; }
    void set_fog_density(float v) { m_fog_density = v; }
    void set_fog_color(float r, float g, float b) { m_fog_color[0] = r; m_fog_color[1] = g; m_fog_color[2] = b; }
    
private:
    GLuint m_fbo = 0;
    GLuint m_color_texture = 0;
    GLuint m_depth_rbo = 0;
    GLuint m_bloom_fbo[2] = {0, 0};
    GLuint m_bloom_texture[2] = {0, 0};
    GLuint m_bloom_shader = 0;
    GLuint m_post_shader = 0;
    int m_width, m_height;
    
    float m_bloom_intensity = 0.5f;
    float m_scanline_intensity = 0.15f;
    float m_vignette_intensity = 0.3f;
    float m_chromatic = 0.0f;
    float m_fog_density = 0.5f;
    float m_fog_color[3] = {0.1f, 0.02f, 0.02f};
    
    void create_framebuffers();
    void render_bloom();
    void render_post();
};

// ============================================================
// WORLD RENDERER (PBR)
// ============================================================

class WorldRenderer {
public:
    WorldRenderer();
    ~WorldRenderer();
    
    bool initialize(SDL_Window* window);
    void shutdown();
    
    // Scene management
    void begin_frame();
    void end_frame();
    
    // Lighting
    void set_sun_direction(float x, float y, float z);
    void set_sun_color(float r, float g, float b);
    void set_ambient_color(float r, float g, float b);
    
    // Camera
    void set_view_matrix(const float* view);
    void set_proj_matrix(const float* proj);
    void set_camera_position(float x, float y, float z);
    
    // Render a model
    void draw_model(Model* model, PBRMaterial* material);
    
    // Render a mesh with material
    void draw_mesh(Mesh* mesh, PBRMaterial* material);
    
    // Skybox
    void draw_sky(const float* sky_color_top, const float* sky_color_bottom);
    
    // Post-processing access
    PostProcessor* post() { return &m_post; }
    
    // Seele asset generation
    SeeleAssetGenerator* assets() { return m_assets; }
    
    // Stats
    int draw_calls() const { return m_draw_calls; }
    int triangle_count() const { return m_triangle_count; }
    
private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_gl_ctx = nullptr;
    
    PBRShader* m_pbr_shader = nullptr;
    PBRShader* m_sky_shader = nullptr;
    
    PostProcessor m_post;
    SeeleAssetGenerator* m_assets = nullptr;
    
    // Matrices
    float m_view[16];
    float m_proj[16];
    float m_cam_pos[3] = {0, 0, 0};
    
    // Lighting
    float m_sun_dir[3] = {0.4f, 1.0f, 0.3f};
    float m_sun_color[3] = {1.0f, 0.95f, 0.85f};
    float m_ambient[3] = {0.15f, 0.25f, 0.15f};
    
    // Stats
    int m_draw_calls = 0;
    int m_triangle_count = 0;
    
    void load_pbr_shader();
    void load_sky_shader();
};

// ============================================================
// MESH GENERATION HELPERS
// ============================================================

Mesh* create_plane(float w, float d, int subdiv);
Mesh* create_box(float w, float h, float d);
Mesh* create_sphere(float radius, int segments, int rings);
Mesh* create_cylinder(float r1, float r2, float h, int segments);
Mesh* create_crystal(float r, float h);
Mesh* create_character(float height, float width, float depth);
Mesh* create_tree(float height, const std::string& type);
Mesh* create_terrain(float size, int res, float height);

} // namespace te
