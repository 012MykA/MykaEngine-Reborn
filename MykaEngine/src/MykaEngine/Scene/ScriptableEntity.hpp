#pragma once

#include "Entity.hpp"

namespace Myka
{
    class ScriptableEntity
    {
        friend class Scene;

    public:
        template <typename T>
        T &GetComponent()
        {
            return m_Entity.GetComponent<T>();
        }

    private:
        Entity m_Entity;
    };
} // namespace Myka
