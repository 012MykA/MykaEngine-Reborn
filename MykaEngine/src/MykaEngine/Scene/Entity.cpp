#include "mykapch.hpp"
#include "Entity.hpp"

namespace Myka
{
    Entity::Entity(entt::entity handle, Scene *scene) : m_EntityHandle(handle), m_Scene(scene)
    {
    }
} // namespace Myka
