#pragma once

// std
#include <xhash>

namespace Myka
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID &) = default;

        operator uint64_t() const { return m_UUID; }

    private:
        uint64_t m_UUID;
    };

} // namespace Myka

namespace std
{
    template <>
    struct hash<Myka::UUID>
    {
        std::size_t operator()(const Myka::UUID &uuid) const
        {
            return hash<uint64_t>()((uint64_t)uuid);
        }
    };
} // namespace std
