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
    vec4 u_AlbedoColor;
    int u_EntityID;
};

struct VertexOutput
{
    vec4 AlbedoColor;
    vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) out VertexOutput v_Output;
layout(location = 3) out flat int v_EntityID;

void main()
{
    v_Output.AlbedoColor = u_AlbedoColor;
    v_Output.Normal = mat3(u_Transform) * a_Normal;
    v_Output.TexCoord = a_TexCoord;
    v_EntityID = u_EntityID;
    
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
    vec4 AlbedoColor;
    vec3 Normal;
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput v_Input;
layout(location = 3) in flat int v_EntityID;

void main()
{
    vec3 normal = normalize(v_Input.Normal);
    
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.8));
    
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.3;
    
    o_Color = v_Input.AlbedoColor * (diffuse + ambient);
    o_EntityID = v_EntityID;
}