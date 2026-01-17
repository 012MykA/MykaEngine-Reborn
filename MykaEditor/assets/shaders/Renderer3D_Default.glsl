#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera
{
    mat4 u_ViewProjection;
};

layout(std140, binding = 1) uniform Entity
{
    mat4 u_Transform;
    vec4 u_Color;
    int u_EntityID;
};

layout(location = 0) out vec4 v_Color;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;
layout(location = 3) out flat int v_EntityID;

void main()
{
    v_Color = u_Color;
    v_Normal = a_Normal;
    v_TexCoord = a_TexCoord;
    v_EntityID = u_EntityID;
    
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

layout(location = 0) in vec4 v_Color;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;
layout(location = 3) in flat int v_EntityID;

void main()
{    
    o_Color = v_Color;
    o_EntityID = v_EntityID;
}