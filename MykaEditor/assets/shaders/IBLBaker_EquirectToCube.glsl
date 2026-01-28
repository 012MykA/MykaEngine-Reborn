#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_Position;
};

layout(location = 0) out vec3 v_WorldPos;

void main() {
    v_WorldPos = a_Position;
    gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_WorldPos;

layout(binding = 0) uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleEquirectangular(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = SampleEquirectangular(normalize(v_WorldPos));
    vec3 color = texture(u_EquirectangularMap, uv).rgb;
    
    o_Color = vec4(color, 1.0);
}