#define GL_GLEXT_PROTOTYPES
#include "sdl2_renderer.h"
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

namespace te {

// ============================================================================
// SDL2 WINDOW
// ============================================================================

SDL_Window* create_sdl_window(const std::string& title, uint32_t w, uint32_t h) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_Window* win = SDL_CreateWindow(title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    return win;
}

// ============================================================================
// GLTexture
// ============================================================================

GLTexture::~GLTexture() {
    if (m_handle) glDeleteTextures(1, &m_handle);
}

bool GLTexture::create(const TextureDesc& desc, const void* data) {
    glGenTextures(1, &m_handle);
    glBindTexture(GL_TEXTURE_2D, m_handle);
    
    GLenum format = (desc.format == TextureFormat::RGBA8) ? GL_RGBA : GL_RGBA;
    GLenum type = GL_UNSIGNED_BYTE;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, desc.width, desc.height, 0, format, type, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return true;
}

// ============================================================================
// GLShader
// ============================================================================

GLShader::~GLShader() {
    if (m_program) glDeleteProgram(m_program);
}

GLuint GLShader::compile_shader(GLenum type, const std::string& source) {
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

bool GLShader::create(const ShaderDesc& desc) {
    // Bytecode path - not used, we use source instead
    return false;
}

bool GLShader::create_from_source(const std::string& vert_src, const std::string& frag_src) {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vert_src);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, frag_src);
    if (!vs || !fs) return false;
    
    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    GLint ok;
    glGetProgramiv(m_program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetProgramInfoLog(m_program, sizeof(buf), nullptr, buf);
        std::cerr << "[Shader] link error: " << buf << "\n";
        return false;
    }
    return true;
}

void GLShader::set_mat4(const std::string& name, const float* mat) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
}

void GLShader::set_vec3(const std::string& name, float x, float y, float z) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniform3f(loc, x, y, z);
}

void GLShader::set_float(const std::string& name, float val) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniform1f(loc, val);
}

void GLShader::set_int(const std::string& name, int val) {
    GLint loc = glGetUniformLocation(m_program, name.c_str());
    if (loc >= 0) glUniform1i(loc, val);
}

// ============================================================================
// SDL2Renderer
// ============================================================================

bool SDL2Renderer::initialize(void* window_handle) {
    if (m_initialized) return true;
    
    if (window_handle) {
        m_window = static_cast<SDL_Window*>(window_handle);
    } else {
        m_window = create_sdl_window("Twilight Elysium", 1280, 720);
    }
    
    m_gl_ctx = SDL_GL_CreateContext(m_window);
    SDL_GL_MakeCurrent(m_window, m_gl_ctx);
    
    // Print GL info
    std::cout << "[Renderer] GL version: " << glGetString(GL_VERSION) << "\n";
    std::cout << "[Renderer] GL renderer: " << glGetString(GL_RENDERER) << "\n";
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    m_initialized = true;
    return true;
}

void SDL2Renderer::shutdown() {
    if (!m_initialized) return;
    
    for (auto t : m_textures) delete t;
    for (auto s : m_shaders) delete s;
    for (auto p : m_passes) delete p;
    m_textures.clear();
    m_shaders.clear();
    m_passes.clear();
    
    if (m_gl_ctx) {
        SDL_GL_DeleteContext(m_gl_ctx);
        m_gl_ctx = nullptr;
    }
    // Don't destroy external window
    m_initialized = false;
}

void SDL2Renderer::begin_frame() {
    glClearColor(0.05f, 0.0f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDL2Renderer::end_frame() {
    SDL_GL_SwapWindow(m_window);
}

Texture* SDL2Renderer::create_texture(const TextureDesc& desc, const void* data) {
    auto tex = new GLTexture();
    if (!tex->create(desc, data)) {
        delete tex;
        return nullptr;
    }
    m_textures.push_back(tex);
    return tex;
}

Shader* SDL2Renderer::create_shader(const ShaderDesc& desc) {
    auto sh = new GLShader();
    if (!sh->create(desc)) {
        delete sh;
        return nullptr;
    }
    m_shaders.push_back(sh);
    return sh;
}

RenderPass* SDL2Renderer::create_render_pass(const RenderPassDesc& desc) {
    // Stub - not needed for basic rendering
    return nullptr;
}

void SDL2Renderer::set_vertex_buffer(Texture* buffer, uint32_t slot) {
    // Stub
}

void SDL2Renderer::draw(uint32_t vertex_count, uint32_t instance_count) {
    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}

Texture* SDL2Renderer::load_texture_from_file(const std::string& path) {
    // Use SDL2_image or stb_image - for now, create a simple colored texture
    // In production, we'd load PNG/JPG here
    TextureDesc desc;
    desc.width = 256;
    desc.height = 256;
    desc.format = TextureFormat::RGBA8;
    
    // Create a red/black .hack-style texture
    std::vector<uint8_t> data(256 * 256 * 4);
    for (size_t i = 0; i < data.size(); i += 4) {
        data[i] = 120 + (rand() % 135);     // R
        data[i+1] = 10 + (rand() % 40);     // G
        data[i+2] = 15 + (rand() % 50);     // B
        data[i+3] = 255;                     // A
    }
    
    return create_texture(desc, data.data());
}

Shader* SDL2Renderer::load_shader_from_files(const std::string& vert_path, const std::string& frag_path) {
    std::ifstream vf(vert_path);
    std::ifstream ff(frag_path);
    if (!vf.good() || !ff.good()) return nullptr;
    
    std::stringstream vs, fs;
    vs << vf.rdbuf();
    fs << ff.rdbuf();
    
    auto sh = new GLShader();
    if (!sh->create_from_source(vs.str(), fs.str())) {
        delete sh;
        return nullptr;
    }
    m_shaders.push_back(sh);
    return sh;
}

} // namespace te
