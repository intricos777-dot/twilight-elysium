#pragma once
#include "renderer.h"
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <string>
#include <vector>
#include <cstdint>

// Use system GL headers (OpenGL 3.3+)

namespace te {

// SDL2 + OpenGL 3.3 real renderer
class SDL2Renderer final : public Renderer {
public:
    SDL2Renderer() = default;
    ~SDL2Renderer() { shutdown(); }

    bool initialize(void* window_handle) override;
    void shutdown() override;
    void begin_frame() override;
    void end_frame() override;

    Texture* create_texture(const TextureDesc& desc, const void* data) override;
    Shader* create_shader(const ShaderDesc& desc) override;
    RenderPass* create_render_pass(const RenderPassDesc& desc) override;

    void set_vertex_buffer(Texture* buffer, uint32_t slot) override;
    void draw(uint32_t vertex_count, uint32_t instance_count = 1) override;

    // Texture loading from file (PNG/JPG)
    Texture* load_texture_from_file(const std::string& path);
    
    // Shader loading from file (vertex + fragment GLSL)
    Shader* load_shader_from_files(const std::string& vert_path, const std::string& frag_path);

    SDL_Window* window() { return m_window; }
    SDL_GLContext gl_context() { return m_gl_ctx; }

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_gl_ctx = nullptr;
    bool m_initialized = false;
    std::vector<Texture*> m_textures;
    std::vector<Shader*> m_shaders;
    std::vector<RenderPass*> m_passes;
};

// ============================================================================
// OPENGL TEXTURE
// ============================================================================

class GLTexture final : public Texture {
public:
    GLTexture() = default;
    ~GLTexture();
    
    bool create(const TextureDesc& desc, const void* data);
    void* get_handle() const override { return reinterpret_cast<void*>(static_cast<uintptr_t>(m_handle)); }
    
    GLuint id() const { return m_handle; }

private:
    GLuint m_handle = 0;
};

// ============================================================================
// OPENGL SHADER
// ============================================================================

class GLShader final : public Shader {
public:
    GLShader() = default;
    ~GLShader();
    
    bool create(const ShaderDesc& desc);
    bool create_from_source(const std::string& vert_src, const std::string& frag_src);
    void* get_handle() const override { return reinterpret_cast<void*>(static_cast<uintptr_t>(m_program)); }
    
    GLuint program() const { return m_program; }
    
    // Uniform setters
    void set_mat4(const std::string& name, const float* mat);
    void set_vec3(const std::string& name, float x, float y, float z);
    void set_float(const std::string& name, float val);
    void set_int(const std::string& name, int val);

private:
    GLuint m_program = 0;
    
    static GLuint compile_shader(GLenum type, const std::string& source);
};

// ============================================================================
// SDL2 WINDOW CREATION (for standalone mode)
// ============================================================================

SDL_Window* create_sdl_window(const std::string& title, uint32_t w, uint32_t h);

} // namespace te
