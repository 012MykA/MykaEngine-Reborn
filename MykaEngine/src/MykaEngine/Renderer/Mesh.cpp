#include "mykapch.hpp"
#include "Mesh.hpp"

namespace Myka
{
    Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
    {
        SetData(vertices, indices);
    }

    void Mesh::SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
    {
        m_VertexArray = VertexArray::Create();

        auto vb = VertexBuffer::Create(vertices.size() * sizeof(Vertex));
        vb->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float3, "a_Normal"},
            {ShaderDataType::Float2, "a_TexCoord"},
        });
        vb->SetData(vertices.data(), vertices.size() * sizeof(Vertex));
        m_VertexArray->AddVertexBuffer(vb);

        auto ib = IndexBuffer::Create(indices.data(), indices.size());
        m_VertexArray->SetIndexBuffer(ib);

        m_IndexCount = indices.size();
    }

} // namespace Myka
