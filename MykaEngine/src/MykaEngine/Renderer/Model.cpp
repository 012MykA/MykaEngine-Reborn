#include "mykapch.hpp"
#include "Model.hpp"

#include "MykaEngine/Core/Timer.hpp"

// tiny_gltf
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

// glm
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Myka
{
    static void ProcessNode(tinygltf::Model &input, tinygltf::Node &node, const glm::mat4 &parentTransform, Model& dst)
    {
        glm::mat4 localTransform = glm::mat4(1.0f);

        if (node.matrix.size() == 16)
        {
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    localTransform[i][j] = (float)node.matrix[i * 4 + j];
                }
            }
        }
        else
        {
            if (node.translation.size() == 3)
            {
                glm::vec3 translation = {(float)node.translation[0], (float)node.translation[1], (float)node.translation[2]};
                localTransform = glm::translate(localTransform, translation);
            }
            if (node.rotation.size() == 4)
            {
                glm::quat q;
                q.x = (float)node.rotation[0];
                q.y = (float)node.rotation[1];
                q.z = (float)node.rotation[2];
                q.w = (float)node.rotation[3];
                localTransform *= glm::toMat4(q);
            }
            if (node.scale.size() == 3)
            {
                glm::vec3 scale = {(float)node.scale[0], (float)node.scale[1], (float)node.scale[2]};
                localTransform = glm::scale(localTransform, scale);
            }
        }

        glm::mat4 globalTransform = parentTransform * localTransform;

        if (node.mesh > -1)
        {
            const tinygltf::Mesh &mesh = input.meshes[node.mesh];
            for (const auto &primitive : mesh.primitives)
            {
                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;

                // Positions
                const float *posPtr = nullptr;
                size_t vertexCount = 0;
                const auto &itPos = primitive.attributes.find("POSITION");
                if (itPos != primitive.attributes.end())
                {
                    const auto &acc = input.accessors[itPos->second];
                    const auto &bv = input.bufferViews[acc.bufferView];
                    posPtr = reinterpret_cast<const float *>(&(input.buffers[bv.buffer].data[acc.byteOffset + bv.byteOffset]));
                    vertexCount = acc.count;
                }

                // Normals
                const float *normPtr = nullptr;
                if (primitive.attributes.count("NORMAL"))
                {
                    const auto &acc = input.accessors[primitive.attributes.at("NORMAL")];
                    const auto &bv = input.bufferViews[acc.bufferView];
                    normPtr = reinterpret_cast<const float *>(&(input.buffers[bv.buffer].data[acc.byteOffset + bv.byteOffset]));
                }

                // Vertices
                for (size_t i = 0; i < vertexCount; ++i)
                {
                    Vertex v;
                    v.Position = glm::make_vec3(&posPtr[i * 3]);
                    v.Normal = normPtr ? glm::make_vec3(&normPtr[i * 3]) : glm::vec3(0.0f);
                    v.TexCoord = glm::vec2(0.0f);
                    vertices.push_back(v);
                }

                // Indices
                const auto &acc = input.accessors[primitive.indices];
                const auto &bv = input.bufferViews[acc.bufferView];
                const auto &buf = input.buffers[bv.buffer];

                if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    const uint32_t *ptr = reinterpret_cast<const uint32_t *>(&buf.data[acc.byteOffset + bv.byteOffset]);
                    indices.insert(indices.end(), ptr, ptr + acc.count);
                }
                else if (acc.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    const uint16_t *ptr = reinterpret_cast<const uint16_t *>(&buf.data[acc.byteOffset + bv.byteOffset]);
                    for (size_t i = 0; i < acc.count; i++)
                        indices.push_back(ptr[i]);
                }

                // Material
                glm::vec4 albedoColor = glm::vec4(1.0f);
                float metallic = 0.0f;
                float roughness = 0.5f;

                if (primitive.material > -1)
                {
                    const tinygltf::Material& material = input.materials[primitive.material];
                    const auto& pbr = material.pbrMetallicRoughness;

                    albedoColor.r = (float)pbr.baseColorFactor[0];
                    albedoColor.g = (float)pbr.baseColorFactor[1];
                    albedoColor.b = (float)pbr.baseColorFactor[2];
                    albedoColor.w = (float)pbr.baseColorFactor[3];

                    metallic = (float)pbr.metallicFactor;
                    roughness = (float)pbr.roughnessFactor;
                }

                Model::Node resNode;
                resNode._Mesh = CreateRef<Mesh>(vertices, indices);
                resNode.LocalTransform = globalTransform;

                resNode._Material = Material::Create();
                resNode._Material->AlbedoColor = albedoColor;
                resNode._Material->Metallic = metallic;
                resNode._Material->Roughness = roughness;

                dst.PushNode(resNode);
            }
        }

        for (int childIdx : node.children)
        {
            ProcessNode(input, input.nodes[childIdx], globalTransform, dst);
        }
    }
    
    Model::Model(const std::filesystem::path &path)
    {
        LoadModel(path);
    }

    Ref<Model> Model::Create(const std::filesystem::path &path)
    {
        return CreateRef<Model>(path);
    }

    void Model::LoadModel(const std::filesystem::path &path)
    {
        MYKA_CORE_TRACE("Loading the model: {0}", path.filename().string());
        Timer timer;

        tinygltf::Model model;
        tinygltf::TinyGLTF loader;
        std::string err, warn;

        bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());
        if (!ret)
            ret = loader.LoadBinaryFromFile(&model, &err, &warn, path.string());

        if (!err.empty())
            MYKA_CORE_ERROR("glTF Error: {0}", err);
        if (!ret)
            return;

        if (!warn.empty())
            MYKA_CORE_WARN("glTF Warning: {0}", warn);

        // Scene
        const tinygltf::Scene &scene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];

        for (int nodeIdx : scene.nodes)
        {
            ProcessNode(model, model.nodes[nodeIdx], glm::mat4(1.0f), *this);
        }

        MYKA_CORE_TRACE("Model load took {0} ms", timer.ElapsedMilliseconds());
    }

} // namespace Myka
