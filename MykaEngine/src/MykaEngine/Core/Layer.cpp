#include "mykapch.hpp"
#include "Layer.hpp"

namespace Myka
{
    Layer::Layer(const std::string &name) : m_DebugName(name) {}

    Layer::~Layer()
    {
    }

} // namespace Myka