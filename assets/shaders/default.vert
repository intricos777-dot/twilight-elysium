#version 460 core
// Twilight Elysium - Default Vertex Shader
// Passthrough vertex shader for basic mesh rendering

layout(location = 0) in vec3 in_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

layout(location = 0) out vec3 v_world_pos;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec2 v_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat3 u_normal_mat;

void main() {
    vec4 world_pos = u_model * vec4(in_pos, 1.0);
    v_world_pos = world_pos.xyz;
    v_normal = u_normal_mat * in_normal;
    v_uv = in_uv;
    gl_Position = u_proj * u_view * world_pos;
}