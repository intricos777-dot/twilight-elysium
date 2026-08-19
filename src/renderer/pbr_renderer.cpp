#define GL_GLEXT_PROTOTYPES
#include "pbr_renderer.h"
#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

namespace te {

// ============================================================
// PBR SHADER
// ============================================================

PBRShader::~PBRShader() {
    if (m_program) glDeleteProgram(m_program);
}

GLuint PBRShader::compile_shader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
        std::cerr << "[Shader] compile error: " << buf << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool PBRShader::create_from_source(const std::string& vert_src, const std::string& frag_src) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vert_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, frag_src);
    if (!vs || !fs) return false;
    
    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    return true;
}

void PBRShader::use() {
    glUseProgram(m_program);
}

void PBRShader::set_mat4(const std::string& name, const float* mat) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void PBRShader::set_vec3(const std::string& name, float x, float y, float z) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniform3f(loc, x, y, z);
}

void PBRShader::set_vec4(const std::string& name, float x, float y, float z, float w) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniform4f(loc, x, y, z, w);
}

void PBRShader::set_float(const std::string& name, float val) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniform1f(loc, val);
}

void PBRShader::set_int(const std::string& name, int val) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniform1i(loc, val);
}

// ============================================================
// MESH
// ============================================================

void Mesh::upload() {
    if (vao == 0) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);
    }
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
    // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)32);
    
    glBindVertexArray(0);
}

void Mesh::draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ibo) glDeleteBuffers(1, &ibo);
    vao = vbo = ibo = 0;
}

// ============================================================
// MODEL
// ============================================================

void Model::draw(PBRShader* shader) {
    for (size_t i = 0; i < meshes.size(); i++) {
        if (i < materials.size() && materials[i]) {
            materials[i]->bind(shader);
        }
        meshes[i]->draw();
        if (i < materials.size() && materials[i]) {
            materials[i]->unbind();
        }
    }
}

// ============================================================
// PBR MATERIAL
// ============================================================

void PBRMaterial::bind(GLuint shader) {
    // Set texture units, uniforms, etc.
}

void PBRMaterial::unbind() {
    // Unbind textures
}

// ============================================================
// POST PROCESSOR
// ============================================================

PostProcessor::PostProcessor() {}
PostProcessor::~PostProcessor() { shutdown(); }

bool PostProcessor::initialize(int width, int height) {
    m_width = width;
    m_height = height;
    create_framebuffers();
    return true;
}

void PostProcessor::shutdown() {
    // Cleanup framebuffers
}

void PostProcessor::create_framebuffers() {
    // Create FBO with color and depth attachments
}

void PostProcessor::begin_scene() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessor::end_scene(PBRShader* shader) {
    // Render post-processing passes
}

// ============================================================
// WORLD RENDERER
// ============================================================

WorldRenderer::WorldRenderer() {}
WorldRenderer::~WorldRenderer() { shutdown(); }

bool WorldRenderer::initialize(SDL_Window* window) {
    m_window = window;
    m_gl_ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, m_gl_ctx);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    load_pbr_shader();
    load_sky_shader();
    
    m_assets = new SeeleAssetGenerator(nullptr);  // TODO: pass AI module
    
    return true;
}

void WorldRenderer::shutdown() {
    delete m_assets;
    if (m_pbr_shader) delete m_pbr_shader;
    if (m_sky_shader) delete m_sky_shader;
    m_post.shutdown();
}

void WorldRenderer::begin_frame() {
    m_draw_calls = 0;
    m_triangle_count = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void WorldRenderer::end_frame() {
    SDL_GL_SwapWindow(m_window);
}

void WorldRenderer::draw_model(Model* model, PBRMaterial* material) {
    m_draw_calls++;
    m_triangle_count += model->meshes[0]->indices.size() / 3;
    model->draw(m_pbr_shader);
}

void WorldRenderer::draw_mesh(Mesh* mesh, PBRMaterial* material) {
    m_draw_calls++;
    m_triangle_count += mesh->indices.size() / 3;
    mesh->draw();
}

void WorldRenderer::load_pbr_shader() {
    // Load from file or use default
    std::string vert_path = "/home/sin/workspace/dot-hack-remake/Content/Assets/sele/shaders/world.vert";
    std::string frag_path = "/home/sin/workspace/dot-hack-remake/Content/Assets/sele/shaders/world_organic.frag";
    
    m_pbr_shader = new PBRShader();
    m_pbr_shader->load_from_files(vert_path, frag_path);
}

void WorldRenderer::load_sky_shader() {
    m_sky_shader = new PBRShader();
    // Load sky shader
}

void WorldRenderer::set_view_matrix(const float* view) {
    memcpy(m_view, view, 16 * sizeof(float));
}

void WorldRenderer::set_proj_matrix(const float* proj) {
    memcpy(m_proj, proj, 16 * sizeof(float));
}

void WorldRenderer::set_camera_position(float x, float y, float z) {
    m_cam_pos[0] = x; m_cam_pos[1] = y; m_cam_pos[2] = z;
}

void WorldRenderer::set_sun_direction(float x, float y, float z) {
    m_sun_dir[0] = x; m_sun_dir[1] = y; m_sun_dir[2] = z;
}

void WorldRenderer::set_sun_color(float r, float g, float b) {
    m_sun_color[0] = r; m_sun_color[1] = g; m_sun_color[2] = b;
}

void WorldRenderer::set_ambient_color(float r, float g, float b) {
    m_ambient[0] = r; m_ambient[1] = g; m_ambient[2] = b;
}

void WorldRenderer::draw_sky(const float* sky_color_top, const float* sky_color_bottom) {
    // Render skybox gradient
}

// ============================================================
// SEELE ASSET GENERATOR
// ============================================================

SeeleAssetGenerator::SeeleAssetGenerator(SeeleAIModule* ai) : m_ai(ai) {}

Model* SeeleAssetGenerator::generate_model(const std::string& description, const std::string& style) {
    // Use Seele AI to generate model based on description
    auto it = m_model_cache.find(description);
    if (it != m_model_cache.end()) return it->second;
    
    Model* model = new Model();
    model->name = description;
    
    // Generate mesh based on style
    if (style == "tower") {
        model->meshes.push_back(create_cylinder(2.0f, 1.0f, 15.0f, 12));
    } else if (style == "character") {
        model->meshes.push_back(create_character(1.8f, 0.4f, 0.3f));
    } else {
        model->meshes.push_back(create_box(2.0f, 2.0f, 2.0f));
    }
    
    // Generate material
    PBRMaterial* mat = generate_material(description, "red");
    model->materials.push_back(mat);
    
    m_model_cache[description] = model;
    return model;
}

PBRMaterial* SeeleAssetGenerator::generate_material(const std::string& description, const std::string& base_color) {
    auto it = m_material_cache.find(description);
    if (it != m_material_cache.end()) return it->second;
    
    PBRMaterial* mat = new PBRMaterial();
    mat->name = description;
    mat->metallic = 0.1f;
    mat->roughness = 0.7f;
    
    // Set color based on description
    if (base_color == "red") {
        mat->albedo[0] = 0.8f; mat->albedo[1] = 0.1f; mat->albedo[2] = 0.1f;
    } else if (base_color == "blue") {
        mat->albedo[0] = 0.1f; mat->albedo[1] = 0.3f; mat->albedo[2] = 0.8f;
    }
    
    m_material_cache[description] = mat;
    return mat;
}

PBRShader* SeeleAssetGenerator::generate_shader(const std::string& zone_type) {
    auto it = m_shader_cache.find(zone_type);
    if (it != m_shader_cache.end()) return it->second;
    
    PBRShader* shader = new PBRShader();
    // Generate shader based on zone type
    m_shader_cache[zone_type] = shader;
    return shader;
}

GLuint SeeleAssetGenerator::generate_texture(const std::string& description, int width, int height) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    // Generate procedural texture
    std::vector<uint8_t> data(width * height * 4);
    // Fill with procedural pattern
    for (int i = 0; i < width * height; i++) {
        data[i*4] = 100 + rand() % 155;
        data[i*4+1] = 10 + rand() % 50;
        data[i*4+2] = 10 + rand() % 50;
        data[i*4+3] = 255;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return tex;
}

GLuint SeeleAssetGenerator::load_original_texture(const std::string& path) {
    // Load from file
    return 0;
}

// ============================================================
// MESH GENERATORS
// ============================================================

Mesh* create_plane(float w, float d, int subdiv) {
    Mesh* mesh = new Mesh();
    float step_w = w / subdiv;
    float step_d = d / subdiv;
    float start_w = -w / 2;
    float start_d = -d / 2;
    
    for (int z = 0; z <= subdiv; z++) {
        for (int x = 0; x <= subdiv; x++) {
            Vertex v;
            v.position[0] = start_w + x * step_w;
            v.position[1] = 0;
            v.position[2] = start_d + z * step_d;
            v.normal[0] = 0; v.normal[1] = 1; v.normal[2] = 0;
            v.uv[0] = (float)x / subdiv;
            v.uv[1] = (float)z / subdiv;
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

Mesh* create_box(float w, float h, float d) {
    Mesh* mesh = new Mesh();
    float hw = w / 2, hh = h / 2, hd = d / 2;
    
    // Front face
    Vertex v1, v2, v3, v4;
    v1.position[0] = -hw; v1.position[1] = -hh; v1.position[2] = hd; v1.uv[0] = 0; v1.uv[1] = 0;
    v2.position[0] = hw; v2.position[1] = -hh; v2.position[2] = hd; v2.uv[0] = 1; v2.uv[1] = 0;
    v3.position[0] = hw; v3.position[1] = hh; v3.position[2] = hd; v3.uv[0] = 1; v3.uv[1] = 1;
    v4.position[0] = -hw; v4.position[1] = hh; v4.position[2] = hd; v4.uv[0] = 0; v4.uv[1] = 1;
    
    // Add faces... (simplified)
    mesh->vertices.push_back(v1);
    mesh->vertices.push_back(v2);
    mesh->vertices.push_back(v3);
    mesh->vertices.push_back(v4);
    
    mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
    mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(3);
    
    mesh->upload();
    return mesh;
}

Mesh* create_sphere(float radius, int segments, int rings) {
    Mesh* mesh = new Mesh();
    
    for (int r = 0; r <= rings; r++) {
        float phi = (float)r / rings * M_PI;
        for (int s = 0; s <= segments; s++) {
            float theta = (float)s / segments * 2 * M_PI;
            
            Vertex v;
            v.position[0] = radius * sin(phi) * cos(theta);
            v.position[1] = radius * cos(phi);
            v.position[2] = radius * sin(phi) * sin(theta);
            v.normal[0] = sin(phi) * cos(theta);
            v.normal[1] = cos(phi);
            v.normal[2] = sin(phi) * sin(theta);
            v.uv[0] = (float)s / segments;
            v.uv[1] = (float)r / rings;
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

Mesh* create_cylinder(float r1, float r2, float h, int segments) {
    Mesh* mesh = new Mesh();
    float hh = h / 2;
    
    for (int i = 0; i <= segments; i++) {
        float a = (float)i / segments * 2 * M_PI;
        float cos_a = cos(a);
        float sin_a = sin(a);
        
        Vertex vb, vt;
        vb.position[0] = cos_a * r1; vb.position[1] = -hh; vb.position[2] = sin_a * r1;
        vt.position[0] = cos_a * r2; vt.position[1] = hh; vt.position[2] = sin_a * r2;
        vb.uv[0] = (float)i / segments; vb.uv[1] = 0;
        vt.uv[0] = (float)i / segments; vt.uv[1] = 1;
        mesh->vertices.push_back(vb);
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

Mesh* create_crystal(float r, float h) {
    return create_cylinder(r * 0.3f, r, h, 6);
}

Mesh* create_character(float height, float width, float depth) {
    return create_box(width, height, depth);
}

Mesh* create_tree(float height, const std::string& type) {
    return create_cylinder(height * 0.1f, height * 0.1f, height, 8);
}

Mesh* create_terrain(float size, int res, float height) {
    return create_plane(size, size, res);
}

} // namespace te
