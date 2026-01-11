#pragma once

#include "Scene.hpp"

namespace Myka
{
    class SceneSerializer
    {
    public:
        SceneSerializer(const Ref<Scene> &scene);

        void SerializeJSON(const std::filesystem::path &filepath);
        void SerializeBinary(const std::filesystem::path &filepath);

        bool DeserializeJSON(const std::filesystem::path &filepath);
        bool DeserializeBinary(const std::filesystem::path &filepath);

    private:
        Ref<Scene> m_Scene;
    };
} // namespace Myka
