#pragma once

#include <string>

namespace SpiritEngine
{
    class FileDialogs
    {
        static std::string OpenFile(const char* filter);
        static std::string SaveFile(const char* filter);
    };
}