#pragma once
#include "sdl2_renderer.h"
#include <GL/glew.h>
#include <string>
#include <vector>
#include <map>

namespace te {

struct RenderTarget {
    GLuint fbo = 0, color = 0, depth = 0, depth_rbo = 0;
    int width = 0, height = 0, samples = 0;
    void create(int w, int h, bool hdr = true);
    void destroy();
    void bind();
    void unbind();
};

struct ShadowMap {
    GLuint fbo = 0, depth = 0;
    int resolution = 2048;
    void create(int res = 2048);
    void destroy();
    void bind();
    void unbind();
};

struct PBRMaterial {
    std::string name;
    GLuint albedo = 0, normal = 0, mr = 0, ao = 0, emissive_tex = 0, height_map = 0;
    float base_color[4] = {1,1,1,1};
    float metalness = 0.0f, roughness = 0.5f, ao_val = 1.0f;
    float emis_color[3] = {0,0,0}, emis_intensity = 1.0f;
    float normal_strength = 1.0f, height_scale = 0.02f;
    float glow_intensity = 0.0f, glow_color[3] = {1.0f,0.2f,0.2f};
    float scanline = 0.0f, fog_density = 0.0f, fog_color[3] = {0.1f,0.02f,0.02f};
    void bind(GLuint shader, int& tex_unit);
    void unbind();
};

struct PBRVertex {
    float position[3], normal[3], tangent[4], uv[2];
};

struct Mesh {
    std::string name;
    std::vector<PBRVertex> verts;
    std::vector<uint32_t> indices;
    GLuint vao = 0, vbo = 0, ibo = 0;
    bool uploaded = false;
    void upload();
    void draw();
    void cleanup();
};

struct Model {
    std::string name;
    std::vector<Mesh*> meshes;
    std::vector<PBRMaterial*> material_list;
    float xform[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
    void draw(GLuint shader);
};

enum class LightType { Directional, Point, Spot };
struct Light {
    LightType type = LightType::Directional;
    float position[3] = {0,0,0}, direction[3] = {0,-1,0};
    float color[3] = {1,1,1}, intensity = 1.0f, range = 10.0f;
    float inner = 0.5f, outer = 1.0f;
    bool shadow = false;
    int cascade = -1;
};

struct Camera {
    float position[3] = {0,5,10}, forward[3] = {0,0,-1}, up[3] = {0,1,0};
    float fov = 60.0f, near_plane = 0.1f, far_plane = 1000.0f;
    float view_mat[16], proj_mat[16];
    void update(int w, int h);
};

class TwilightUERenderer {
public:
    TwilightUERenderer();
    ~TwilightUERenderer();
    
    bool initialize(SDL_Window* window);
    void shutdown();
    
    void begin_frame();
    void end_frame();
    
    void draw_mesh(Mesh* mesh, PBRMaterial* material, const float* model_matrix);
    void draw_model(Model* model);
    void draw_skybox(const float* top, const float* bottom);
    void apply_post_processing(RenderTarget* target);
    
    void set_shadows(bool v) { m_shadows = v; }
    void set_bloom(bool v) { m_bloom = v; }
    void set_ssao(bool v) { m_ssao = v; }
    void set_aa(int v) { m_aa = v; }
    
    ShadowMap* get_shadows() { return &m_shadow_map; }
    Camera* get_camera() { return &m_camera; }
    
    int get_draw_calls() const { return m_draw_calls; }
    int get_tri_count() const { return m_triangles; }
    float get_frame_time() const { return m_frame_time; }
    
    void add_light(const Light& light);
    void clear_lights();
    void set_ambient(float r, float g, float b);
    
    PBRMaterial* create_material(const std::string& name);
    PBRMaterial* get_material(const std::string& name);
    Mesh* create_mesh(const std::string& name);
    Mesh* get_mesh(const std::string& name);
    GLuint compile_shader(const std::string& vert, const std::string& frag);
    void draw_quad();
    
private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_gl_ctx = nullptr;
    
    GLuint m_pbr = 0, m_shadow = 0, m_sky = 0, m_tonemap = 0;
    
    RenderTarget m_hdr;
    RenderTarget m_bloom_mips[6];
    RenderTarget m_ssao_target, m_ssao_blur_target;
    ShadowMap m_shadow_map;
    Camera m_camera;
    
    std::vector<Light> m_lights;
    float m_ambient[3] = {0.1f, 0.1f, 0.1f};
    
    std::map<std::string, PBRMaterial*> m_materials;
    std::map<std::string, Mesh*> m_meshes;
    
    GLuint m_quad_vao = 0, m_quad_vbo = 0;
    
    bool m_shadows = true, m_bloom = true, m_ssao = true;
    int m_aa = 1;
    
    float m_bloom_intensity = 0.5f, m_exposure = 1.0f;
    float m_contrast = 1.0f, m_saturation = 1.0f;
    
    int m_draw_calls = 0, m_triangles = 0;
    float m_frame_time = 0.0f;
    
    void create_quad();
    void load_shaders();
    void create_targets(int w, int h);
    void do_bloom();
    void do_ssao();
    void do_tonemap();
    
    void set_mat4(GLuint s, const char* n, const float* m);
    void set_vec3(GLuint s, const char* n, float x, float y, float z);
    void set_float(GLuint s, const char* n, float v);
    void set_int(GLuint s, const char* n, int v);
};

} // namespace te
