#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdio>
#include <cstring>

#include "twilight_ue_renderer.h"

namespace te {

// ============================================================
// RENDER TARGET
// ============================================================

void RenderTarget::create(int w, int h, bool hdr) {
    width = w; height = h;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    GLenum fmt = hdr ? GL_RGBA16F : GL_RGBA8;
    
    glGenTextures(1, &color);
    glBindTexture(GL_TEXTURE_2D, color);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, GL_RGBA, hdr ? GL_HALF_FLOAT : GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
    
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::destroy() {
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (color) glDeleteTextures(1, &color);
    if (depth) glDeleteTextures(1, &depth);
}

void RenderTarget::bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
void RenderTarget::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

// ============================================================
// SHADOW MAP
// ============================================================

void ShadowMap::create(int res) {
    resolution = res;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, res, res, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float bc[] = {1,1,1,1};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bc);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::destroy() {
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (depth) glDeleteTextures(1, &depth);
}

void ShadowMap::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, resolution, resolution);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
}

void ShadowMap::unbind() { glCullFace(GL_BACK); }

// ============================================================
// MESH
// ============================================================

void Mesh::upload() {
    if (!vao) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(PBRVertex), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)40);
    glBindVertexArray(0);
    uploaded = true;
}

void Mesh::draw() {
    if (!uploaded) upload();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ibo) glDeleteBuffers(1, &ibo);
}

// ============================================================
// TWILIGHT UE RENDERER
// ============================================================

TwilightUERenderer::TwilightUERenderer() {}
TwilightUERenderer::~TwilightUERenderer() { shutdown(); }

bool TwilightUERenderer::initialize(SDL_Window* window) {
    m_window = window;
    m_gl_ctx = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, m_gl_ctx);
    
    glewInit();
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    m_shadow_map.create(2048);
    load_shaders();
    create_quad();
    
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    create_targets(w, h);
    
    std::printf("[TwilightUE] Initialized: %s\n", glGetString(GL_VERSION));
    return true;
}

void TwilightUERenderer::shutdown() {
    m_shadow_map.destroy();
    m_hdr.destroy();
    for (auto& [n,m] : m_materials) delete m;
    for (auto& [n,m] : m_meshes) delete m;
    m_materials.clear(); m_meshes.clear();
    SDL_GL_DeleteContext(m_gl_ctx);
}

void TwilightUERenderer::begin_frame() {
    m_draw_calls = 0;
    m_triangles = 0;
}

void TwilightUERenderer::end_frame() {
    SDL_GL_SwapWindow(m_window);
}

void TwilightUERenderer::draw_mesh(Mesh* mesh, PBRMaterial* material, const float* model_matrix) {
    if (!mesh || !material) return;
    
    glUseProgram(m_pbr);
    set_mat4(m_pbr, "u_model", model_matrix);
    set_mat4(m_pbr, "u_view", m_camera.view_mat);
    set_mat4(m_pbr, "u_proj", m_camera.proj_mat);
    set_vec3(m_pbr, "u_albedo", material->base_color[0], material->base_color[1], material->base_color[2]);
    set_float(m_pbr, "u_metallic", material->metalness);
    set_float(m_pbr, "u_roughness", material->roughness);
    set_float(m_pbr, "u_ao", material->ao_val);
    set_vec3(m_pbr, "u_emissive", material->emis_color[0], material->emis_color[1], material->emis_color[2]);
    set_float(m_pbr, "u_emissive_intensity", material->emis_intensity);
    set_vec3(m_pbr, "u_cam_pos", m_camera.position[0], m_camera.position[1], m_camera.position[2]);
    set_vec3(m_pbr, "u_sun_dir", 0.5f, -1.0f, 0.3f);
    set_vec3(m_pbr, "u_sun_color", 1.0f, 0.95f, 0.9f);
    set_float(m_pbr, "u_sun_intensity", 2.0f);
    set_vec3(m_pbr, "u_ambient", m_ambient[0], m_ambient[1], m_ambient[2]);
    
    int tex = 0;
    material->bind(m_pbr, tex);
    mesh->draw();
    m_draw_calls++;
    m_triangles += mesh->indices.size() / 3;
}

void TwilightUERenderer::draw_model(Model* model) {
    int tex = 0;
    for (size_t idx = 0; idx < model->meshes.size(); idx++) {
        PBRMaterial* mat = (idx < model->material_list.size()) ? model->material_list[idx] : nullptr;
        if (mat) mat->bind(m_pbr, tex);
        model->meshes[idx]->draw();
        if (mat) mat->unbind();
    }
}

void TwilightUERenderer::create_quad() {
    float verts[] = {-1,-1,0,0, 1,-1,1,0, 1,1,1,1, -1,1,0,1};
    uint16_t idx[] = {0,1,2, 0,2,3};
    glGenVertexArrays(1, &m_quad_vao);
    glGenBuffers(1, &m_quad_vbo);
    glBindVertexArray(m_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);
    glBindVertexArray(0);
}

void TwilightUERenderer::draw_quad() {
    glBindVertexArray(m_quad_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

void TwilightUERenderer::load_shaders() {
    const std::string pbr_vert = R"(
#version 330 core
layout(location=0) in vec3 a_pos;
layout(location=1) in vec3 a_normal;
layout(location=2) in vec4 a_tangent;
layout(location=3) in vec2 a_uv;
uniform mat4 u_model, u_view, u_proj;
out vec3 v_world, v_normal;
out vec2 v_uv; out mat3 v_tbn;
void main() {
    vec4 wp = u_model * vec4(a_pos, 1.0);
    v_world = wp.xyz; v_uv = a_uv;
    vec3 N = normalize(mat3(u_model) * a_normal);
    vec3 T = normalize(mat3(u_model) * a_tangent.xyz);
    v_tbn = mat3(T, cross(N,T) * a_tangent.w, N);
    gl_Position = u_proj * u_view * wp;
}
)";
    const std::string pbr_frag = R"(
#version 330 core
in vec3 v_world, v_normal; in vec2 v_uv; in mat3 v_tbn;
out vec4 frag;
uniform vec3 u_albedo, u_emissive, u_cam_pos, u_sun_dir, u_sun_color, u_ambient;
uniform float u_metallic, u_roughness, u_ao, u_emissive_intensity, u_sun_intensity;
uniform sampler2D u_albedo_map, u_normal_map, u_mr_map, u_ao_map, u_emissive_map;
uniform int u_has_albedo, u_has_normal, u_has_mr, u_has_ao, u_has_emissive;
const float PI = 3.14159265;
float D(float NoH, float r){float a=r*r,d=(NoH*NoH*(a-1)+1);return a/(PI*d*d+1e-4);}
float G(float NoV, float r){float k=(r+1);k=k*k/8;return NoV/(NoV*(1-k)+k);}
vec3 Fsch(float ct, vec3 F0){return F0+(1-F0)*pow(1-ct,5);}
void main(){
    vec3 alb = u_albedo; float met = u_metallic, rough = u_roughness, ao = u_ao;
    vec3 emis = u_emissive * u_emissive_intensity;
    if(u_has_albedo==1) alb *= texture(u_albedo_map,v_uv).rgb;
    vec3 N = normalize(v_normal);
    if(u_has_normal==1) N = normalize(v_tbn*(texture(u_normal_map,v_uv).rgb*2-1));
    if(u_has_mr==1){vec2 mr=texture(u_mr_map,v_uv).gb;met*=mr.x;rough*=mr.y;}
    if(u_has_ao==1) ao *= texture(u_ao_map,v_uv).r;
    if(u_has_emissive==1) emis += texture(u_emissive_map,v_uv).rgb;
    vec3 V = normalize(u_cam_pos - v_world);
    vec3 L = normalize(-u_sun_dir); vec3 H = normalize(V+L);
    float NoV=max(dot(N,V),0), NoL=max(dot(N,L),0), NoH=max(dot(N,H),0), VoH=max(dot(H,V),0);
    vec3 F0 = mix(vec3(0.04), alb, met);
    float Dval = D(NoH, rough);
    float Gval = G(NoV, rough) * G(NoL, rough);
    vec3 Fval = Fsch(VoH, F0);
    vec3 spec = (Dval*Gval*Fval)/(4*NoV*NoL+1e-4);
    vec3 kD = (1-Fval)*(1-met);
    vec3 diff = kD*alb/PI;
    vec3 color = (diff+spec)*u_sun_color*u_sun_intensity*NoL + u_ambient*alb*ao + emis;
    frag = vec4(color, 1);
}
)";
    m_pbr = compile_shader(pbr_vert, pbr_frag);
    
    const std::string shadow_vert = R"(
#version 330 core
layout(location=0) in vec3 a_pos;
uniform mat4 u_model, u_light_vp;
void main(){ gl_Position = u_light_vp * u_model * vec4(a_pos, 1.0); }
)";
    const std::string shadow_frag = R"(#version 330 core
out vec4 o; void main(){ o=vec4(0); }
)";
    m_shadow = compile_shader(shadow_vert, shadow_frag);
    
    const std::string sky_vert = R"(
#version 330 core
layout(location=0) in vec2 a_pos;
out vec2 v_uv;
void main(){ v_uv=a_pos; gl_Position=vec4(a_pos,0.9999,1.0); }
)";
    const std::string sky_frag = R"(
#version 330 core
in vec2 v_uv; out vec4 frag;
uniform vec3 u_top, u_bottom;
void main(){ frag=vec4(mix(u_bottom,u_top,v_uv.y*0.5+0.5),1); }
)";
    m_sky = compile_shader(sky_vert, sky_frag);
    
    const std::string tonemap_frag = R"(
#version 330 core
in vec2 v_uv; out vec4 frag;
uniform sampler2D scene, bloom;
uniform float exposure, bloom_int, contrast, saturation;
vec3 aces(vec3 x){return clamp((x*(2.51*x+0.03))/(x*(2.43*x+0.59)+0.14),0,1);}
void main(){
    vec3 c = texture(scene,v_uv).rgb + texture(bloom,v_uv).rgb*bloom_int;
    c *= exposure; c = aces(c); c = pow(c, vec3(1/2.2));
    c = (c-0.5)*contrast+0.5;
    float l = dot(c, vec3(0.2126,0.7152,0.0722));
    c = mix(vec3(l), c, saturation);
    frag = vec4(c,1);
}
)";
    m_tonemap = compile_shader(sky_vert, tonemap_frag);
    
    std::printf("[TwilightUE] Shaders loaded\n");
}

GLuint TwilightUERenderer::compile_shader(const std::string& vs, const std::string& fs) {
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    const char* pv = vs.c_str();
    glShaderSource(v, 1, &pv, nullptr);
    glCompileShader(v);
    
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
    const char* pf = fs.c_str();
    glShaderSource(f, 1, &pf, nullptr);
    glCompileShader(f);
    
    GLuint p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f);
    glLinkProgram(p);
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

void TwilightUERenderer::set_mat4(GLuint s, const char* n, const float* m) {
    GLint l = glGetUniformLocation(s, n); if(l>=0) glUniformMatrix4fv(l,1,GL_FALSE,m);
}
void TwilightUERenderer::set_vec3(GLuint s, const char* n, float x, float y, float z) {
    GLint l = glGetUniformLocation(s, n); if(l>=0) glUniform3f(l,x,y,z);
}
void TwilightUERenderer::set_float(GLuint s, const char* n, float v) {
    GLint l = glGetUniformLocation(s, n); if(l>=0) glUniform1f(l,v);
}
void TwilightUERenderer::set_int(GLuint s, const char* n, int v) {
    GLint l = glGetUniformLocation(s, n); if(l>=0) glUniform1i(l,v);
}

void TwilightUERenderer::create_targets(int w, int h) {
    m_hdr.create(w, h, true);
    for (int i = 0; i < 6; i++) {
        int bw = w >> (i+1); int bh = h >> (i+1);
        if(bw<1) bw=1; if(bh<1) bh=1;
        m_bloom_mips[i].create(bw, bh, true);
    }
    m_ssao_target.create(w/2, h/2, false);
    m_ssao_blur_target.create(w/2, h/2, false);
}

PBRMaterial* TwilightUERenderer::create_material(const std::string& name) {
    if (m_materials.count(name)) return m_materials[name];
    PBRMaterial* m = new PBRMaterial(); m->name = name;
    return m_materials[name] = m;
}

PBRMaterial* TwilightUERenderer::get_material(const std::string& name) {
    auto it = m_materials.find(name); return it != m_materials.end() ? it->second : nullptr;
}

Mesh* TwilightUERenderer::create_mesh(const std::string& name) {
    if (m_meshes.count(name)) return m_meshes[name];
    Mesh* m = new Mesh(); m->name = name;
    return m_meshes[name] = m;
}

Mesh* TwilightUERenderer::get_mesh(const std::string& name) {
    auto it = m_meshes.find(name); return it != m_meshes.end() ? it->second : nullptr;
}

void TwilightUERenderer::draw_skybox(const float* top, const float* bottom) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_sky);
    set_vec3(m_sky, "u_top", top[0], top[1], top[2]);
    set_vec3(m_sky, "u_bottom", bottom[0], bottom[1], bottom[2]);
    draw_quad();
    glEnable(GL_DEPTH_TEST);
}

void TwilightUERenderer::apply_post_processing(RenderTarget* target) {
    do_bloom();
    do_tonemap();
}

void TwilightUERenderer::do_bloom() {}
void TwilightUERenderer::do_ssao() {}

void TwilightUERenderer::do_tonemap() {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_tonemap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_hdr.color);
    set_int(m_tonemap, "scene", 0);
    set_float(m_tonemap, "exposure", m_exposure);
    set_float(m_tonemap, "bloom_int", m_bloom_intensity);
    set_float(m_tonemap, "contrast", m_contrast);
    set_float(m_tonemap, "saturation", m_saturation);
    draw_quad();
    glEnable(GL_DEPTH_TEST);
}

void Camera::update(int sw, int sh) {
    float aspect = (float)sw/(float)sh, tanh = tanf(fov*3.14159f/360.0f);
    memset(proj_mat, 0, 64);
    proj_mat[0]=1/(aspect*tanh); proj_mat[5]=1/tanh;
    float near = near_plane, far = far_plane;
    proj_mat[10]=-(far+near)/(far-near);
    proj_mat[11]=-1; proj_mat[14]=-(2*far*near)/(far-near);
}

void PBRMaterial::bind(GLuint shader, int& tex_unit) {
    GLint loc;
    if (albedo) {
        glActiveTexture(GL_TEXTURE0+tex_unit); glBindTexture(GL_TEXTURE_2D, albedo);
        loc=glGetUniformLocation(shader,"u_albedo_map"); if(loc>=0) glUniform1i(loc,tex_unit);
        loc=glGetUniformLocation(shader,"u_has_albedo"); if(loc>=0) glUniform1i(loc,1);
        tex_unit++;
    }
    if (normal) {
        glActiveTexture(GL_TEXTURE0+tex_unit); glBindTexture(GL_TEXTURE_2D, normal);
        loc=glGetUniformLocation(shader,"u_normal_map"); if(loc>=0) glUniform1i(loc,tex_unit);
        loc=glGetUniformLocation(shader,"u_has_normal"); if(loc>=0) glUniform1i(loc,1);
        tex_unit++;
    }
    if (mr) {
        glActiveTexture(GL_TEXTURE0+tex_unit); glBindTexture(GL_TEXTURE_2D, mr);
        loc=glGetUniformLocation(shader,"u_mr_map"); if(loc>=0) glUniform1i(loc,tex_unit);
        loc=glGetUniformLocation(shader,"u_has_mr"); if(loc>=0) glUniform1i(loc,1);
        tex_unit++;
    }
    if (ao) {
        glActiveTexture(GL_TEXTURE0+tex_unit); glBindTexture(GL_TEXTURE_2D, ao);
        loc=glGetUniformLocation(shader,"u_ao_map"); if(loc>=0) glUniform1i(loc,tex_unit);
        loc=glGetUniformLocation(shader,"u_has_ao"); if(loc>=0) glUniform1i(loc,1);
        tex_unit++;
    }
    if (emissive_tex) {
        glActiveTexture(GL_TEXTURE0+tex_unit); glBindTexture(GL_TEXTURE_2D, emissive_tex);
        loc=glGetUniformLocation(shader,"u_emissive_map"); if(loc>=0) glUniform1i(loc,tex_unit);
        loc=glGetUniformLocation(shader,"u_has_emissive"); if(loc>=0) glUniform1i(loc,1);
        tex_unit++;
    }
}

void PBRMaterial::unbind() {}

void Model::draw(GLuint shader) {
    int tex = 0;
    for (size_t idx = 0; idx < meshes.size(); idx++) {
        PBRMaterial* mat = (idx < material_list.size()) ? material_list[idx] : nullptr;
        if (mat) mat->bind(shader, tex);
        meshes[idx]->draw();
        if (mat) mat->unbind();
    }
}

void TwilightUERenderer::add_light(const Light& light) { m_lights.push_back(light); }
void TwilightUERenderer::clear_lights() { m_lights.clear(); }

void TwilightUERenderer::set_ambient(float r, float g, float b) {
    m_ambient[0] = r; m_ambient[1] = g; m_ambient[2] = b;
}

} // namespace te
