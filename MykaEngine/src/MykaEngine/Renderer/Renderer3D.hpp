#pragma once

#include "EditorCamera.hpp"
#include "Mesh.hpp"

namespace Myka
{
    class Renderer3D
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const EditorCamera& camera);
        static void EndScene();

        static void DrawMesh(const Ref<Mesh>& mesh, const glm::mat4& transform, const glm::vec4& color, int entityID = -1);


        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t IndicesCount = 0;
        };

        static void ResetStats();
        static Statistics GetStats();
    };
} // namespace Myka
