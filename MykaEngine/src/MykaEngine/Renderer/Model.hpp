#pragma once

#include "Mesh.hpp"
#include "Material.hpp"

#include <glm/glm.hpp>

namespace Myka
{
    class Model
    {
    public:
        Model(const std::filesystem::path &path);

        struct Node
        {
            std::string Name;
            Ref<Mesh> _Mesh;
            Ref<Material> _Material;
            glm::mat4 LocalTransform;
            std::vector<Node> Children;
        };

        const std::vector<Node> &GetNodes() const { return m_Nodes; }
        const std::filesystem::path &GetPath() const { return m_Path; }

        void PushNode(const Node& node) { m_Nodes.push_back(node); }

        static Ref<Model> Create(const std::filesystem::path &path);

    private:
        void LoadModel(const std::filesystem::path &path);
        
        std::filesystem::path m_Path;
        std::vector<Node> m_Nodes;
    };
} // namespace Myka
