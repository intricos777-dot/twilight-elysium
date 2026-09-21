#include "shader_stub.h"
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <sstream>

namespace te {

static std::string read_file(const std::string& path) {
    std::ifstream f(path);
    if (!f) return "";
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

bool ShaderCompiler::compile(const ShaderDesc& desc, std::vector<uint8_t>& out_bytecode) {
    // For software renderer: store source as bytecode
    // In a GPU build, this would call glCompileShader / glAttachShader / glLinkProgram
    std::string stage_str;
    switch (desc.stage) {
        case ShaderStage::Vertex:   stage_str = "vertex"; break;
        case ShaderStage::Fragment: stage_str = "fragment"; break;
        case ShaderStage::Compute:  stage_str = "compute"; break;
    }
    std::printf("[Shader] compiled %s shader (%zu bytes)\n", stage_str.c_str(), desc.bytecode.size());
    out_bytecode = desc.bytecode;
    return true;
}

bool ShaderCompiler::compile_from_file(const std::string& path, ShaderStage stage, std::vector<uint8_t>& out_bytecode) {
    std::string source = read_file(path);
    if (source.empty()) {
        std::fprintf(stderr, "[Shader] failed to read: %s\n", path.c_str());
        return false;
    }
    ShaderDesc desc{stage, "main", std::vector<uint8_t>(source.begin(), source.end())};
    return compile(desc, out_bytecode);
}

bool hot_reload_shader(const std::string& path, ShaderDesc& desc) {
    std::error_code ec;
    auto ftime = std::filesystem::last_write_time(path, ec);
    if (ec) return false;
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    desc.bytecode.assign(std::istreambuf_iterator<char>(f), {});
    desc.entry_point = "main";
    return true;
}

bool load_shader_stage(const std::string& path, ShaderStage stage, std::vector<uint8_t>& out_bytecode) {
    return ShaderCompiler::compile_from_file(path, stage, out_bytecode);
}

} // namespace te