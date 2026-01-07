#pragma once

#include "Scene.hpp"

namespace Myka
{
    class SceneSerializer
    {
    public:
        SceneSerializer(const Ref<Scene> &scene);

        void SerializeJSON(const std::string &filepath);
        void SerializeBinary(const std::string &filepath);

        bool DeserializeJSON(const std::string &filepath);
        bool DeserializeBinary(const std::string &filepath);

    private:
        Ref<Scene> m_Scene;
    };
} // namespace Myka
