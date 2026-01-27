#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Projection;
    vec3 u_Position;
};

layout(location = 0) out vec3 v_TexCoord;

void main() {
    v_TexCoord = a_Position;

    mat4 rotView = mat4(mat3(u_View));
    vec4 clipPos = u_Projection * rotView * vec4(a_Position, 1.0);

    gl_Position = clipPos.xyww;
}

#type fragment
#version 460 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

layout(location = 0) in vec3 v_TexCoord;

layout(std140, binding = 3) uniform SkyboxSettings {
    float u_Intensity;
};

layout(binding = 31) uniform samplerCube u_Skybox;

void main() {
    vec3 envColor = texture(u_Skybox, v_TexCoord).rgb;

    envColor *= u_Intensity;

    // Gamma
    vec3 color = envColor / (envColor + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    o_Color = vec4(color, 1.0);
    o_EntityID = -1;
}