#define GL_GLEXT_PROTOTYPES
#include "seele_world_renderer.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace te {

// ============================================================================
// WORLD MESH
// ============================================================================

void WorldMesh::upload() {
    if (vao == 0) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ibo);
    }
    
    glBindVertexArray(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(WorldVertex), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WorldVertex), (void*)0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(WorldVertex), (void*)12);
    // UV
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(WorldVertex), (void*)24);
    
    glBindVertexArray(0);
}

void WorldMesh::draw() {
    if (texture_id) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void WorldMesh::cleanup() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ibo) glDeleteBuffers(1, &ibo);
    vao = vbo = ibo = 0;
}

// ============================================================================
// SEELE WORLD RENDERER
// ============================================================================

SeeleWorldRenderer::SeeleWorldRenderer() {
    m_view.identity();
    m_proj.identity();
}

SeeleWorldRenderer::~SeeleWorldRenderer() {
    shutdown();
}

bool SeeleWorldRenderer::initialize(SDL2Renderer* renderer, seele::SeeleAIModule* ai) {
    m_renderer = renderer;
    m_ai = ai;
    
    if (ai) {
        m_content_gen = new hackgu::HackguContentGenerator(ai);
    }
    
    if (!load_shaders()) {
        std::cerr << "[SeeleWorld] failed to load shaders\n";
        return false;
    }
    
    std::printf("[SeeleWorld] initialized (AI=%s)\n", ai ? "yes" : "no");
    return true;
}

void SeeleWorldRenderer::shutdown() {
    for (auto m : m_meshes) { m->cleanup(); delete m; }
    for (auto o : m_objects) delete o;
    for (auto t : m_textures) delete t;
    m_meshes.clear();
    m_objects.clear();
    m_textures.clear();
    
    delete m_content_gen;
    m_content_gen = nullptr;
}

bool SeeleWorldRenderer::load_shaders() {
    // Use the original game shaders if available, otherwise use generated ones
    std::string vert_path = "/home/sin/workspace/dot-hack-remake/Content/Assets/seele/vol1_i/shaders/world.vert";
    std::string frag_path = "/home/sin/workspace/dot-hack-remake/Content/Assets/seele/vol1_i/shaders/world_organic.frag";
    
    m_world_shader = static_cast<GLShader*>(m_renderer->load_shader_from_files(vert_path, frag_path));
    if (!m_world_shader) {
        // Fallback: generate shader from Seele AI
        if (m_ai) {
            auto shader_src = m_ai->generate_world_shader("organic", "ground");
            m_world_shader = new GLShader();
            m_world_shader->create_from_source(shader_src.vertex_source, shader_src.fragment_source);
        }
    }
    
    return m_world_shader != nullptr;
}

// ============================================================================
// ORIGINAL GAME ASSETS
// ============================================================================

bool SeeleWorldRenderer::load_original_assets(const std::string& asset_dir) {
    // Load original game textures from the extracted data
    // For now, generate placeholder textures based on original color palettes
    std::printf("[SeeleWorld] loading original assets from: %s\n", asset_dir.c_str());
    
    // Generate textures based on original game zones
    for (auto& zone : m_zones) {
        generate_zone_textures(zone);
    }
    
    return true;
}

void SeeleWorldRenderer::generate_zone_textures(const Zone& zone) {
    // Generate textures using Seele AI based on zone type
    std::string desc;
    if (zone.type == "root_town") desc = "urban digital plaza, .hack root town aesthetic";
    else if (zone.type == "dungeon") desc = "dark dungeon cave, data corruption aesthetic";
    else if (zone.type == "field") desc = "open field with data streams, .hack aesthetic";
    else if (zone.type == "palace") desc = "ethereal palace, glowing data architecture";
    else desc = "digital landscape, .hack world aesthetic";
    
    auto asset = m_ai->generate_texture(desc, 512, 512);
    
    // Upload to GPU
    TextureDesc td;
    td.width = asset.width;
    td.height = asset.height;
    td.format = TextureFormat::RGBA8;
    
    auto tex = m_renderer->create_texture(td, asset.data.data());
    if (tex) {
        auto gltex = static_cast<GLTexture*>(tex);
        // Store texture ID in our list
        m_textures.push_back(new seele::ProceduralAsset(std::move(asset)));
    }
}

// ============================================================================
// MESH GENERATION
// ============================================================================

WorldMesh* SeeleWorldRenderer::create_plane(float w, float d, int subdiv) {
    auto mesh = new WorldMesh();
    
    float step_w = w / subdiv;
    float step_d = d / subdiv;
    float start_w = -w / 2;
    float start_d = -d / 2;
    
    for (int z = 0; z <= subdiv; z++) {
        for (int x = 0; x <= subdiv; x++) {
            WorldVertex v;
            v.x = start_w + x * step_w;
            v.y = 0.0f;
            v.z = start_d + z * step_d;
            v.nx = 0.0f; v.ny = 1.0f; v.nz = 0.0f;
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
    m_meshes.push_back(mesh);
    return mesh;
}

WorldMesh* SeeleWorldRenderer::create_box(float w, float h, float d) {
    auto mesh = new WorldMesh();
    
    float hw = w / 2, hh = h / 2, hd = d / 2;
    
    // Front face
    WorldVertex v;
    v.nx = 0; v.ny = 0; v.nz = 1;
    v.y = -hh; v.ny = 0;
    
    // Simplified box - just add 8 corners
    float positions[8][3] = {
        {-hw, -hh, hd}, {hw, -hh, hd}, {hw, hh, hd}, {-hw, hh, hd},
        {-hw, -hh, -hd}, {hw, -hh, -hd}, {hw, hh, -hd}, {-hw, hh, -hd}
    };
    
    // Front
    for (int i = 0; i < 4; i++) {
        v.x = positions[i][0]; v.y = positions[i][1]; v.z = positions[i][2];
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    // Add more faces... (simplified for brevity)
    // Just add indices for front face
    mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
    mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(3);
    
    mesh->upload();
    m_meshes.push_back(mesh);
    return mesh;
}

WorldMesh* SeeleWorldRenderer::create_terrain(float size, int res, float height) {
    auto mesh = new WorldMesh();
    
    float step = size / res;
    float start = -size / 2;
    
    for (int z = 0; z <= res; z++) {
        for (int x = 0; x <= res; x++) {
            WorldVertex v;
            v.x = start + x * step;
            v.z = start + z * step;
            
            // Generate height using simple noise
            float fx = (float)x / res * 6.28f;
            float fz = (float)z / res * 6.28f;
            v.y = sin(fx) * cos(fz) * height + sin(fx * 2.3f + fz * 1.7f) * height * 0.3f;
            
            v.nx = 0.0f; v.ny = 1.0f; v.nz = 0.0f;
            v.u = (float)x / res;
            v.v = (float)z / res;
            mesh->vertices.push_back(v);
        }
    }
    
    for (int z = 0; z < res; z++) {
        for (int x = 0; x < res; x++) {
            int i0 = z * (res + 1) + x;
            int i1 = i0 + 1;
            int i2 = i0 + (res + 1);
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
    m_meshes.push_back(mesh);
    return mesh;
}

WorldMesh* SeeleWorldRenderer::create_tower(float r1, float r2, float h) {
    auto mesh = new WorldMesh();
    int segments = 16;
    
    for (int i = 0; i <= segments; i++) {
        float a = (float)i / segments * 6.2831853f;
        float cos_a = cos(a);
        float sin_a = sin(a);
        
        // Bottom vertex
        WorldVertex vb;
        vb.x = cos_a * r1;
        vb.y = 0.0f;
        vb.z = sin_a * r1;
        vb.nx = cos_a; vb.ny = 0.5f; vb.nz = sin_a;
        vb.u = (float)i / segments;
        vb.v = 0.0f;
        mesh->vertices.push_back(vb);
        
        // Top vertex
        WorldVertex vt;
        vt.x = cos_a * r2;
        vt.y = h;
        vt.z = sin_a * r2;
        vt.nx = cos_a; vt.ny = 0.5f; vt.nz = sin_a;
        vt.u = (float)i / segments;
        vt.v = 1.0f;
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
    m_meshes.push_back(mesh);
    return mesh;
}

WorldMesh* SeeleWorldRenderer::create_pillar(float r, float h) {
    return create_tower(r, r, h);
}

WorldMesh* SeeleWorldRenderer::create_character(float height) {
    auto mesh = new WorldMesh();
    
    // Simple character: box body + sphere head
    float body_w = 0.4f * height;
    float body_h = 0.5f * height;
    float body_d = 0.2f * height;
    float head_r = 0.15f * height;
    
    // Body (simplified box)
    WorldVertex v;
    v.nx = 0; v.ny = 0; v.nz = 1;
    
    // Front face of body
    float hw = body_w / 2;
    float hh = body_h / 2;
    float hd = body_d / 2;
    float y_base = head_r * 2;
    
    // 4 front vertices
    for (int i = 0; i < 4; i++) {
        v.x = (i == 1 || i == 2) ? hw : -hw;
        v.y = (i == 2 || i == 3) ? y_base + body_h : y_base;
        v.z = hd;
        v.u = (i == 1 || i == 2) ? 1.0f : 0.0f;
        v.v = (i == 2 || i == 3) ? 1.0f : 0.0f;
        mesh->vertices.push_back(v);
    }
    
    mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
    mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(3);
    
    mesh->upload();
    m_meshes.push_back(mesh);
    return mesh;
}

// ============================================================================
// WORLD GENERATION
// ============================================================================

void SeeleWorldRenderer::generate_world(const std::string& disc_id, const std::string& zone_name) {
    std::printf("[SeeleWorld] generating world for disc=%s zone=%s\n", disc_id.c_str(), zone_name.c_str());
    
    // Create default zone if none loaded
    if (m_zones.empty()) {
        Zone z;
        z.id = disc_id + "_" + zone_name;
        z.name = zone_name;
        z.type = "field";
        z.sky[0] = 25; z.sky[1] = 5; z.sky[2] = 10;
        z.ground[0] = 140; z.ground[1] = 20; z.ground[2] = 30;
        z.fog = 60;
        z.structures = {"tower", "monolith", "shrine"};
        z.npcs = {"Kite", "BlackRose"};
        z.monsters = {"Skeleton", "Data Bug"};
        m_zones.push_back(z);
    }
    
    // Generate textures
    for (auto& zone : m_zones) {
        generate_zone_textures(zone);
        generate_structure_textures(zone);
        generate_character_textures(zone);
    }
    
    // Place objects
    for (auto& zone : m_zones) {
        place_terrain(zone);
        place_structures(zone);
        place_npcs(zone);
        place_monsters(zone);
    }
    
    std::printf("[SeeleWorld] world generated: %d objects, %d meshes, %d textures\n",
        object_count(), mesh_count(), texture_count());
}

void SeeleWorldRenderer::place_terrain(const Zone& zone) {
    auto terrain = create_terrain(100.0f, 32, 2.0f);
    
    WorldObject* obj = new WorldObject();
    obj->name = "terrain";
    obj->type = "terrain";
    obj->x = 0; obj->y = 0; obj->z = 0;
    obj->scale = 1.0f;
    obj->mesh = terrain;
    obj->texture = m_textures.empty() ? nullptr : m_textures[0];
    m_objects.push_back(obj);
}

void SeeleWorldRenderer::place_structures(const Zone& zone) {
    int idx = 0;
    for (const auto& s : zone.structures) {
        WorldMesh* mesh = nullptr;
        
        if (s == "tower") mesh = create_tower(1.5f, 1.0f, 8.0f);
        else if (s == "monolith") mesh = create_box(2.0f, 6.0f, 0.5f);
        else if (s == "shrine") mesh = create_pillar(0.8f, 4.0f);
        else if (s == "pillar") mesh = create_pillar(0.5f, 5.0f);
        else mesh = create_box(2.0f, 3.0f, 2.0f);
        
        WorldObject* obj = new WorldObject();
        obj->name = s + "_" + std::to_string(idx);
        obj->type = "structure";
        obj->x = (idx % 3 - 1) * 15.0f;
        obj->y = 0;
        obj->z = (idx / 3 - 1) * 15.0f;
        obj->scale = 1.0f;
        obj->mesh = mesh;
        obj->texture = m_textures.empty() ? nullptr : m_textures[0];
        m_objects.push_back(obj);
        idx++;
    }
}

void SeeleWorldRenderer::place_npcs(const Zone& zone) {
    int idx = 0;
    for (const auto& npc : zone.npcs) {
        auto mesh = create_character(1.8f);
        
        WorldObject* obj = new WorldObject();
        obj->name = npc;
        obj->type = "npc";
        obj->x = (float)(idx * 8 - 10);
        obj->y = 0;
        obj->z = (float)(idx * 5 + 5);
        obj->scale = 1.0f;
        obj->mesh = mesh;
        obj->texture = m_textures.size() > 1 ? m_textures[1] : nullptr;
        m_objects.push_back(obj);
        idx++;
    }
}

void SeeleWorldRenderer::place_monsters(const Zone& zone) {
    int idx = 0;
    for (const auto& mon : zone.monsters) {
        auto mesh = create_box(1.5f, 1.5f, 1.5f);
        
        WorldObject* obj = new WorldObject();
        obj->name = mon;
        obj->type = "monster";
        obj->x = (float)(idx * 12 - 20);
        obj->y = 0.75f;
        obj->z = (float)(-idx * 8 - 15);
        obj->scale = 1.0f;
        obj->mesh = mesh;
        obj->texture = m_textures.size() > 2 ? m_textures[2] : nullptr;
        m_objects.push_back(obj);
        idx++;
    }
}

void SeeleWorldRenderer::generate_structure_textures(const Zone& zone) {
    if (!m_ai) return;
    
    auto asset = m_ai->generate_texture(
        "tower structure, .hack digital architecture, glowing red accents", 256, 256);
    
    TextureDesc td;
    td.width = asset.width;
    td.height = asset.height;
    td.format = TextureFormat::RGBA8;
    m_renderer->create_texture(td, asset.data.data());
    m_textures.push_back(new seele::ProceduralAsset(std::move(asset)));
}

void SeeleWorldRenderer::generate_character_textures(const Zone& zone) {
    if (!m_ai) return;
    
    auto asset = m_ai->generate_texture(
        "character portrait, .hack style, data corruption aesthetic", 256, 256);
    
    TextureDesc td;
    td.width = asset.width;
    td.height = asset.height;
    td.format = TextureFormat::RGBA8;
    m_renderer->create_texture(td, asset.data.data());
    m_textures.push_back(new seele::ProceduralAsset(std::move(asset)));
}

// ============================================================================
// ZONE DATA LOADING
// ============================================================================

bool SeeleWorldRenderer::load_zone_data(const std::string& json_path, const std::string& disc_id) {
    std::ifstream f(json_path);
    if (!f.good()) return false;
    
    std::stringstream ss;
    ss << f.rdbuf();
    std::string json = ss.str();
    
    // Simple JSON parser for zones array
    size_t zones_start = json.find("\"zones\"");
    if (zones_start == std::string::npos) return false;
    
    size_t arr_start = json.find('[', zones_start);
    if (arr_start == std::string::npos) return false;
    arr_start++;
    
    while (arr_start < json.size()) {
        // Skip whitespace
        while (arr_start < json.size() && (json[arr_start] == ' ' || json[arr_start] == '\n' || json[arr_start] == '\r' || json[arr_start] == '\t' || json[arr_start] == ','))
            arr_start++;
        if (arr_start >= json.size() || json[arr_start] == ']') break;
        if (json[arr_start] != '{') { arr_start++; continue; }
        
        Zone zone;
        arr_start++;
        
        while (arr_start < json.size()) {
            while (arr_start < json.size() && (json[arr_start] == ' ' || json[arr_start] == '\n' || json[arr_start] == '\t')) arr_start++;
            if (arr_start >= json.size() || json[arr_start] == '}') { arr_start++; break; }
            if (json[arr_start] != '"') { arr_start++; continue; }
            
            // Read key
            arr_start++;
            size_t key_start = arr_start;
            while (arr_start < json.size() && json[arr_start] != '"') arr_start++;
            std::string key = json.substr(key_start, arr_start - key_start);
            arr_start++;
            
            while (arr_start < json.size() && (json[arr_start] == ' ' || json[arr_start] == ':')) arr_start++;
            if (arr_start >= json.size()) break;
            
            if (json[arr_start] == '"') {
                arr_start++;
                size_t val_start = arr_start;
                while (arr_start < json.size() && json[arr_start] != '"') arr_start++;
                std::string val = json.substr(val_start, arr_start - val_start);
                arr_start++;
                
                if (key == "id") zone.id = val;
                else if (key == "name") zone.name = val;
                else if (key == "type") zone.type = val;
            } else if (json[arr_start] == '[') {
                arr_start++;
                if (key == "sky" || key == "ground") {
                    int idx = 0;
                    while (arr_start < json.size() && json[arr_start] != ']') {
                        while (arr_start < json.size() && (json[arr_start] == ' ' || json[arr_start] == ',')) arr_start++;
                        if (json[arr_start] >= '0' && json[arr_start] <= '9') {
                            int num = 0;
                            while (arr_start < json.size() && json[arr_start] >= '0' && json[arr_start] <= '9') {
                                num = num * 10 + (json[arr_start] - '0');
                                arr_start++;
                            }
                            if (key == "sky" && idx < 3) zone.sky[idx] = num;
                            if (key == "ground" && idx < 3) zone.ground[idx] = num;
                            idx++;
                        } else arr_start++;
                    }
                } else {
                    std::vector<std::string>* target = nullptr;
                    if (key == "structures") target = &zone.structures;
                    else if (key == "npcs") target = &zone.npcs;
                    else if (key == "monsters") target = &zone.monsters;
                    
                    if (target) {
                        while (arr_start < json.size() && json[arr_start] != ']') {
                            while (arr_start < json.size() && (json[arr_start] == ' ' || json[arr_start] == ',')) arr_start++;
                            if (json[arr_start] == '"') {
                                arr_start++;
                                size_t s = arr_start;
                                while (arr_start < json.size() && json[arr_start] != '"') arr_start++;
                                target->push_back(json.substr(s, arr_start - s));
                                arr_start++;
                            } else arr_start++;
                        }
                    }
                }
                if (arr_start < json.size() && json[arr_start] == ']') arr_start++;
            } else if (json[arr_start] >= '0' && json[arr_start] <= '9') {
                int num = 0;
                while (arr_start < json.size() && json[arr_start] >= '0' && json[arr_start] <= '9') {
                    num = num * 10 + (json[arr_start] - '0');
                    arr_start++;
                }
                if (key == "fog") zone.fog = num;
            }
        }
        
        if (!zone.id.empty() && zone.id.find(disc_id) != std::string::npos) {
            m_zones.push_back(zone);
        }
    }
    
    std::printf("[SeeleWorld] loaded %zu zones for %s\n", m_zones.size(), disc_id.c_str());
    return !m_zones.empty();
}

// ============================================================================
// CAMERA
// ============================================================================

void SeeleWorldRenderer::rotate_camera(float dx, float dy) {
    m_rot_y += dx * 0.01f;
    m_rot_x += dy * 0.01f;
    m_rot_x = std::max(-1.5f, std::min(1.5f, m_rot_x));
}

void SeeleWorldRenderer::move_camera(float dx, float dz) {
    m_cam_x += dx * cos(m_rot_y) + dz * sin(m_rot_y);
    m_cam_z += -dx * sin(m_rot_y) + dz * cos(m_rot_y);
}

void SeeleWorldRenderer::zoom_camera(float delta) {
    m_zoom *= (1.0f + delta * 0.1f);
    m_zoom = std::max(0.1f, std::min(10.0f, m_zoom));
}

void SeeleWorldRenderer::update_view_matrix() {
    // Build view matrix from camera position and rotation
    float cx = cos(m_rot_x), sx = sin(m_rot_x);
    float cy = cos(m_rot_y), sy = sin(m_rot_y);
    
    // Look-at calculation
    float fx = sx * sy;
    float fy = -cx;
    float fz = sx * cy;
    
    // Right vector
    float rx = cy;
    float ry = 0;
    float rz = -sy;
    
    // Up vector = right x forward
    float ux = ry * fz - rz * fy;
    float uy = rz * fx - rx * fz;
    float uz = rx * fy - ry * fx;
    
    // Distance from camera based on zoom
    float dist = 20.0f * m_zoom;
    float cam_px = m_cam_x - fx * dist;
    float cam_py = m_cam_y - fy * dist;
    float cam_pz = m_cam_z - fz * dist;
    
    // Build view matrix (column-major)
    m_view.data[0] = rx; m_view.data[4] = ux; m_view.data[8] = -fx; m_view.data[12] = -(rx*cam_px + ux*cam_py + (-fx)*cam_pz);
    m_view.data[1] = ry; m_view.data[5] = uy; m_view.data[9] = -fy; m_view.data[13] = -(ry*cam_px + uy*cam_py + (-fy)*cam_pz);
    m_view.data[2] = rz; m_view.data[6] = uz; m_view.data[10] = -fz; m_view.data[14] = -(rz*cam_px + uz*cam_py + (-fz)*cam_pz);
    m_view.data[3] = 0; m_view.data[7] = 0; m_view.data[11] = 0; m_view.data[15] = 1;
}

void SeeleWorldRenderer::update_proj_matrix(float fov, float aspect) {
    float tan_half = tan(fov / 2.0f);
    m_proj.identity();
    m_proj.data[0] = 1.0f / (aspect * tan_half);
    m_proj.data[5] = 1.0f / tan_half;
    m_proj.data[10] = -1.0f;
    m_proj.data[11] = -1.0f;
    m_proj.data[14] = -2.0f * 0.1f; // near plane
    m_proj.data[15] = 0;
}

// ============================================================================
// RENDER
// ============================================================================

void SeeleWorldRenderer::render(float dt) {
    // Update camera
    update_view_matrix();
    update_proj_matrix(1.0f, 16.0f / 9.0f);
    
    // Set shader uniforms
    if (m_world_shader) {
        m_world_shader->set_mat4("u_view", m_view.data);
        m_world_shader->set_mat4("u_proj", m_proj.data);
        m_world_shader->set_mat4("u_model", mtx::identity().data);
        m_world_shader->set_vec3("u_light_dir", 0.4f, 1.0f, 0.3f);
        m_world_shader->set_int("u_texture", 0);
    }
    
    // Draw all objects
    for (auto obj : m_objects) {
        if (obj->mesh) {
            // Set model matrix for this object
            mtx model = mtx::translation({obj->x, obj->y, obj->z});
            model = mtx::scale({obj->scale, obj->scale, obj->scale});
            m_world_shader->set_mat4("u_model", model.data);
            
            // Bind texture
            if (obj->texture) {
                // Find corresponding GL texture
                // For now, use first texture
                if (!m_textures.empty()) {
                    // Texture binding would happen here
                }
            }
            
            obj->mesh->draw();
        }
    }
}

} // namespace te
