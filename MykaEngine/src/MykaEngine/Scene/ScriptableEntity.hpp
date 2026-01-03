#pragma once

#include "Entity.hpp"

namespace Myka
{
    class ScriptableEntity
    {
        friend class Scene;

    public:
        virtual ~ScriptableEntity() = default;

        template <typename T>
        T &GetComponent()
        {
            return m_Entity.GetComponent<T>();
        }

    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(Timestep ts) {}

    private:
        Entity m_Entity;
    };
} // namespace Myka
