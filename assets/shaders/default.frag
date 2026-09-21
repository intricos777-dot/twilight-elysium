#version 460 core
// Twilight Elysium - Default Fragment Shader
// Simple gradient shader with PBR-ish lighting for software renderer fallback

layout(location = 0) in vec3 v_world_pos;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 out_color;

uniform vec3 u_albedo;
uniform float u_metallic;
uniform float u_roughness;
uniform vec3 u_light_pos;
uniform vec3 u_light_color;
uniform float u_time;

void main() {
    vec3 N = normalize(v_normal);
    vec3 L = normalize(u_light_pos - v_world_pos);
    vec3 V = normalize(-v_world_pos);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    // Simple Lambert diffuse
    vec3 diffuse = u_albedo * (NdotL * 0.6 + 0.4);

    // Specular highlight
    float spec = pow(max(dot(N, H), 0.0), mix(16.0, 256.0, 1.0 - u_roughness));
    vec3 specular = u_light_color * spec * (1.0 - u_metallic);

    // Subtle pulse over time
    float pulse = sin(u_time * 0.5) * 0.05 + 0.95;

    out_color = vec4((diffuse + specular) * pulse, 1.0);
}