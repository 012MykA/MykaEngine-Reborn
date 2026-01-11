#pragma once

#include <optional>
#include <filesystem>

namespace Myka
{
    class FileDialogs
    {
    public:
        static std::optional<std::filesystem::path> OpenFile(const char *filter);
        static std::optional<std::filesystem::path> SaveFile(const char *filter);
    };
} // namespace Myka
