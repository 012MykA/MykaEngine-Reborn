#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
    vec3 u_Position;
};

layout(std140, binding = 1) uniform Entity {
    mat4 u_Transform;
    vec4 u_AlbedoColor;
    int u_AlbedoTexIndex;
    int u_MetRoughTexIndex;
    int u_EntityID;
};

struct VertexOutput {
    vec3 Normal;
    vec2 TexCoord;
    vec3 WorldPos;
};

layout(location = 0) out VertexOutput v_Output;
layout(location = 3) out flat int v_EntityID;

void main() {
    vec4 worldPos = u_Transform * vec4(a_Position, 1.0);
    v_Output.WorldPos = worldPos.xyz;
    v_Output.Normal = normalize(mat3(u_Transform) * a_Normal);
    v_Output.TexCoord = a_TexCoord;
    v_EntityID = u_EntityID;

    gl_Position = u_ViewProjection * worldPos;
}

#type fragment
#version 460 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput {
    vec3 Normal;
    vec2 TexCoord;
    vec3 WorldPos;
};

layout(location = 0) in VertexOutput v_Input;
layout(location = 3) in flat int v_EntityID;

layout(std140, binding = 0) uniform Camera {
    mat4 u_ViewProjection;
    vec3 u_Position;
};

layout(std140, binding = 1) uniform Entity {
    mat4 u_Transform;
    vec4 u_AlbedoColor;
    int u_AlbedoTexIndex;
    int u_MetRoughTexIndex;
    int u_EntityID;
};

struct PointLight {
    vec3 Position;
    float Intensity;
    vec3 Color;
    float Radius;
    float Falloff;
    float _pad;
};

struct DirectionalLight {
    vec3 Direction;
    float Intensity;
    vec3 Color;
    int CastShadows;
};

struct SpotLight {
    vec3 Position;
    float Intensity;
    vec3 Direction;
    float Range;
    vec3 Color;
    float InnerCutoff;
    float OuterCutoff;
    float _pad;
};

layout(std140, binding = 2) uniform Lights {
    PointLight u_PointLights[10];
    DirectionalLight u_DirLights[10];
    SpotLight u_SpotLights[10];
    uint u_PointCount;
    uint u_DirCount;
    uint u_SpotCount;
};

layout(binding = 0) uniform sampler2D u_Textures[32];

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return nom / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    return GeometrySchlickGGX(max(dot(N, V), 0.0), roughness) *
        GeometrySchlickGGX(max(dot(N, L), 0.0), roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    vec4 texColor = texture(u_Textures[u_AlbedoTexIndex], v_Input.TexCoord) * u_AlbedoColor;
    if(texColor.a < 0.1)
        return;

    vec4 arm = texture(u_Textures[u_MetRoughTexIndex], v_Input.TexCoord);
    float roughness = clamp(arm.g, 0.05, 1.0);
    float metallic = arm.b;

    vec3 N = normalize(v_Input.Normal);
    vec3 V = normalize(u_Position - v_Input.WorldPos);
    vec3 F0 = mix(vec3(0.04), texColor.rgb, metallic);

    vec3 Lo = vec3(0.0);

    for(int i = 0; i < int(u_PointCount); ++i) {
        vec3 L = normalize(u_PointLights[i].Position - v_Input.WorldPos);
        vec3 H = normalize(V + L);

        float dist = length(u_PointLights[i].Position - v_Input.WorldPos);
        float attenuation = u_PointLights[i].Intensity / (dist * dist + 1.0);
        float distanceFade = smoothstep(u_PointLights[i].Radius, 0.0, dist);
        vec3 radiance = u_PointLights[i].Color * attenuation * distanceFade;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        Lo += (kD * texColor.rgb / PI + specular) * radiance * max(dot(N, L), 0.0);
    }

    for(int i = 0; i < int(u_DirCount); ++i) {
        vec3 L = normalize(-u_DirLights[i].Direction);
        vec3 H = normalize(V + L);
        vec3 radiance = u_DirLights[i].Color * u_DirLights[i].Intensity;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 specular = (NDF * G * F) / (4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001);
        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        Lo += (kD * texColor.rgb / PI + specular) * radiance * max(dot(N, L), 0.0);
    }

    for(int i = 0; i < int(u_SpotCount); ++i) {
        vec3 L_vec = u_SpotLights[i].Position - v_Input.WorldPos;
        float dist = length(L_vec);

        if(dist > u_SpotLights[i].Range)
            continue;

        vec3 L = normalize(L_vec);
        vec3 H = normalize(V + L);

        float attenuation = u_SpotLights[i].Intensity / (dist * dist + 1.0);
        float distanceFade = clamp(1.0 - dist / u_SpotLights[i].Range, 0.0, 1.0);

        float theta = dot(L, normalize(-u_SpotLights[i].Direction));
        float epsilon = u_SpotLights[i].InnerCutoff - u_SpotLights[i].OuterCutoff;
        float spotIntensity = clamp((theta - u_SpotLights[i].OuterCutoff) / epsilon, 0.0, 1.0);

        vec3 radiance = u_SpotLights[i].Color * attenuation * spotIntensity * distanceFade;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        Lo += (kD * texColor.rgb / PI + specular) * radiance * max(dot(N, L), 0.0);
    }

    for(int i = 0; i < int(u_SpotCount); ++i) {
        vec3 L_vec = u_SpotLights[i].Position - v_Input.WorldPos;
        float dist = length(L_vec);

        if(dist > u_SpotLights[i].Range)
            continue;

        vec3 L = normalize(L_vec);
        vec3 H = normalize(V + L);

        float attenuation = u_SpotLights[i].Intensity / (dist * dist + 1.0);
        float distanceFade = clamp(1.0 - dist / u_SpotLights[i].Range, 0.0, 1.0);

        float theta = dot(L, normalize(-u_SpotLights[i].Direction));
        float epsilon = u_SpotLights[i].InnerCutoff - u_SpotLights[i].OuterCutoff;
        float spotIntensity = clamp((theta - u_SpotLights[i].OuterCutoff) / epsilon, 0.0, 1.0);

        vec3 radiance = u_SpotLights[i].Color * attenuation * spotIntensity * distanceFade;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
        Lo += (kD * texColor.rgb / PI + specular) * radiance * max(dot(N, L), 0.0);
    }

    vec3 ambient = vec3(0.03) * texColor.rgb;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    o_Color = vec4(color, texColor.a);
    o_EntityID = v_EntityID;
}
