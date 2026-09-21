#include "blender_asset_loader.h"
#include <fstream>
#include <iostream>
#include <array>
#include <cstring>

namespace te::seele {

// ============================================================================
// SIMPLE JSON PARSER (for manifest)
// ============================================================================

namespace {

// Minimal JSON array/object parser for the manifest format
struct JsonValue {
    enum Type { Null, String, Number, Object, Array, Boolean };
    Type type = Null;
    std::string string_val;
    double number_val = 0;
    bool bool_val = false;
    std::unordered_map<std::string, JsonValue> object_val;
    std::vector<JsonValue> array_val;
};

class SimpleJsonParser {
public:
    static JsonValue parse(const std::string& text) {
        size_t pos = 0;
        return parse_value(text, pos);
    }
    
private:
    static void skip_ws(const std::string& text, size_t& pos) {
        while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\n' || 
               text[pos] == '\r' || text[pos] == '\t')) pos++;
    }
    
    static JsonValue parse_value(const std::string& text, size_t& pos) {
        skip_ws(text, pos);
        if (pos >= text.size()) return {};
        
        char c = text[pos];
        if (c == '"') return parse_string(text, pos);
        if (c == '{') return parse_object(text, pos);
        if (c == '[') return parse_array(text, pos);
        if (c == 't' || c == 'f') return parse_bool(text, pos);
        if (c == 'n') return parse_null(text, pos);
        return parse_number(text, pos);
    }
    
    static JsonValue parse_string(const std::string& text, size_t& pos) {
        pos++; // skip opening quote
        std::string result;
        while (pos < text.size() && text[pos] != '"') {
            if (text[pos] == '\\' && pos + 1 < text.size()) {
                pos++;
                switch (text[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += text[pos]; break;
                }
            } else {
                result += text[pos];
            }
            pos++;
        }
        pos++; // skip closing quote
        JsonValue v;
        v.type = JsonValue::String;
        v.string_val = result;
        return v;
    }
    
    static JsonValue parse_number(const std::string& text, size_t& pos) {
        size_t start = pos;
        if (text[pos] == '-') pos++;
        while (pos < text.size() && (text[pos] == '.' || (text[pos] >= '0' && text[pos] <= '9'))) pos++;
        JsonValue v;
        v.type = JsonValue::Number;
        v.number_val = std::stod(text.substr(start, pos - start));
        return v;
    }
    
    static JsonValue parse_object(const std::string& text, size_t& pos) {
        pos++; // skip {
        JsonValue v;
        v.type = JsonValue::Object;
        skip_ws(text, pos);
        if (pos < text.size() && text[pos] == '}') { pos++; return v; }
        
        while (true) {
            skip_ws(text, pos);
            JsonValue key = parse_string(text, pos);
            skip_ws(text, pos);
            pos++; // skip :
            JsonValue value = parse_value(text, pos);
            v.object_val[key.string_val] = value;
            skip_ws(text, pos);
            if (pos >= text.size() || text[pos] == '}') break;
            pos++; // skip ,
        }
        pos++; // skip }
        return v;
    }
    
    static JsonValue parse_array(const std::string& text, size_t& pos) {
        pos++; // skip [
        JsonValue v;
        v.type = JsonValue::Array;
        skip_ws(text, pos);
        if (pos < text.size() && text[pos] == ']') { pos++; return v; }
        
        while (true) {
            v.array_val.push_back(parse_value(text, pos));
            skip_ws(text, pos);
            if (pos >= text.size() || text[pos] == ']') break;
            pos++; // skip ,
        }
        pos++; // skip ]
        return v;
    }
    
    static JsonValue parse_bool(const std::string& text, size_t& pos) {
        JsonValue v;
        v.type = JsonValue::Boolean;
        if (text.substr(pos, 4) == "true") { v.bool_val = true; pos += 4; }
        else { v.bool_val = false; pos += 5; }
        return v;
    }
    
    static JsonValue parse_null(const std::string& text, size_t& pos) {
        pos += 4;
        return {};
    }
};

} // anonymous namespace

// ============================================================================
// BLENDER ASSET LOADER IMPLEMENTATION
// ============================================================================

bool BlenderAssetLoader::load_manifest(const std::string& manifest_path) {
    std::ifstream file(manifest_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open manifest: " << manifest_path << std::endl;
        return false;
    }
    
    std::string text((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();
    
    JsonValue root = SimpleJsonParser::parse(text);
    if (root.type != JsonValue::Object) {
        std::cerr << "Manifest root is not an object" << std::endl;
        return false;
    }
    
    // Parse models array
    auto it = root.object_val.find("models");
    if (it == root.object_val.end() || it->second.type != JsonValue::Array) {
        std::cerr << "No models array in manifest" << std::endl;
        return false;
    }
    
    m_manifest.models.clear();
    for (const auto& model_val : it->second.array_val) {
        if (model_val.type != JsonValue::Object) continue;
        
        BlenderModel model;
        auto name_it = model_val.object_val.find("name");
        if (name_it != model_val.object_val.end() && name_it->second.type == JsonValue::String)
            model.name = name_it->second.string_val;
        
        auto path_it = model_val.object_val.find("path");
        if (path_it != model_val.object_val.end() && path_it->second.type == JsonValue::String)
            model.path = path_it->second.string_val;
        
        auto format_it = model_val.object_val.find("format");
        if (format_it != model_val.object_val.end() && format_it->second.type == JsonValue::String)
            model.format = format_it->second.string_val;
        
        auto vert_it = model_val.object_val.find("vertices");
        if (vert_it != model_val.object_val.end() && vert_it->second.type == JsonValue::Number)
            model.vertex_count = static_cast<uint32_t>(vert_it->second.number_val);
        
        auto face_it = model_val.object_val.find("faces");
        if (face_it != model_val.object_val.end() && face_it->second.type == JsonValue::Number)
            model.face_count = static_cast<uint32_t>(face_it->second.number_val);
        
        m_manifest.models.push_back(model);
    }
    
    // Parse count
    auto count_it = root.object_val.find("count");
    if (count_it != root.object_val.end() && count_it->second.type == JsonValue::Number)
        m_manifest.count = static_cast<uint32_t>(count_it->second.number_val);
    else
        m_manifest.count = static_cast<uint32_t>(m_manifest.models.size());
    
    return true;
}

bool BlenderAssetLoader::load_manifest_from_dir(const std::string& export_dir) {
    m_export_dir = export_dir;
    return load_manifest(export_dir + "/seele_manifest.json");
}

bool BlenderAssetLoader::upload_model(const std::string& name) {
    const BlenderModel* model = m_manifest.find(name);
    if (!model) {
        std::cerr << "Model not found: " << name << std::endl;
        return false;
    }
    return upload_model(*model);
}

bool BlenderAssetLoader::upload_model(const BlenderModel& model) {
    // Placeholder: In production, this would:
    // 1. Parse GLB using tinygltf
    // 2. Extract vertex/index buffers
    // 3. Create OpenGL VAO/VBO/EBO or Vulkan buffers
    // 4. Upload to GPU
    
    // For now, mark as uploaded
    const_cast<BlenderModel&>(model).uploaded = true;
    m_uploaded_count++;
    
    std::cout << "Uploaded model: " << model.name 
              << " (" << model.vertex_count << " verts, " 
              << model.face_count << " faces)" << std::endl;
    return true;
}

bool BlenderAssetLoader::upload_all_models() {
    for (const auto& model : m_manifest.models) {
        if (!upload_model(model)) return false;
    }
    return true;
}

void BlenderAssetLoader::draw_model(const std::string& name) {
    // Placeholder: In production, bind VAO and draw
    // glBindVertexArray(model.vao);
    // glDrawElements(GL_TRIANGLES, model.index_count, GL_UNSIGNED_INT, 0);
}

bool BlenderAssetLoader::load_texture_set(const std::string& style, const std::string& base_path) {
    // Placeholder: Load PBR textures (albedo, normal, roughness, metallic, AO)
    // using stb_image or similar, then upload to GPU as 2D textures
    std::cout << "Loading texture set: " << style << " from " << base_path << std::endl;
    return true;
}

// ============================================================================
// BLENDER PIPELINE IMPLEMENTATION
// ============================================================================

bool BlenderPipeline::blender_available() {
    return std::system("which blender > /dev/null 2>&1") == 0;
}

PipelineConfig BlenderPipeline::default_config(const std::string& project_dir) {
    PipelineConfig config;
    config.blender_path = "/usr/bin/blender";
    config.bridge_script = project_dir + "/tools/blender_seele_bridge.py";
    config.export_dir = project_dir + "/assets/blender";
    config.model_type = "all";
    config.texture_size = 512;
    config.seed = 42;
    return config;
}

bool BlenderPipeline::run_generation(const PipelineConfig& config) {
    if (!blender_available()) {
        std::cerr << "Blender not found at: " << config.blender_path << std::endl;
        return false;
    }
    
    // Build command
    std::string cmd = config.blender_path + " --background --python " + config.bridge_script +
                      " -- --model-type " + config.model_type +
                      " --output-dir " + config.export_dir +
                      " --texture-size " + std::to_string(config.texture_size);
    
    if (config.seed != 0) {
        cmd += " --seed " + std::to_string(config.seed);
    }
    
    std::cout << "Running Blender pipeline: " << cmd << std::endl;
    int result = std::system(cmd.c_str());
    
    if (result != 0) {
        std::cerr << "Blender pipeline failed with code: " << result << std::endl;
        return false;
    }
    
    std::cout << "Blender pipeline complete. Output: " << config.export_dir << std::endl;
    return true;
}

} // namespace te::seele
