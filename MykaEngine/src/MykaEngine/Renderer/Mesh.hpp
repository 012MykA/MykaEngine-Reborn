#pragma once

#include "VertexArray.hpp"

#include <glm/glm.hpp>

namespace Myka
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;
    };

    class Mesh
    {
    public:
        Mesh() = default;
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

        void SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

        inline const Ref<VertexArray> &GetVertexArray() const { return m_VertexArray; }
        inline uint32_t GetIndexCount() const { return m_IndexCount; }

    private:
        Ref<VertexArray> m_VertexArray;
        uint32_t m_IndexCount;
    };
} // namespace Myka
