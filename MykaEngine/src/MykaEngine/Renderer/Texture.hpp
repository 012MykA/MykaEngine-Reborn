#pragma once

#include "MykaEngine/Core.hpp"

#include <cinttypes>
#include <string>

namespace Myka
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void Bind() const = 0;

    private:
    };

    class Texture2D : public Texture
    {
    public:
        static Ref<Texture2D> Create(const std::string &path);
    };
} // namespace Myka
