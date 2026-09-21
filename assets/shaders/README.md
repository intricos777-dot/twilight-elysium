# Twilight Elysium Shader Assets

## Directory Structure
```
assets/shaders/
├── default.vert          # Default vertex shader (PBR passthrough)
├── default.frag          # Default fragment shader (gradient/PBR)
├── corruption.frag       # Corruption/data shader (seele_shader.cpp)
├── toon.frag             # Toon shading shader (seele_shader.cpp)
└── cache/                # Compiled shader cache directory
    └── .gitkeep
```

## Shader Pipeline
The engine loads shaders from this directory at runtime. The `ShaderCompiler`
class in `src/shader/` compiles GLSL source to SPIR-V or native GPU bytecode.
The `FrameGraph` in `src/pipeline/` manages render pass attachments and
subpass dependencies.
