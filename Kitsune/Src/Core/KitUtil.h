#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "KitLogs.h"

namespace Kitsune
{
    class KitUtil
    {
    public:
        static std::vector<char> ReadFile(const std::string& file_path)
        {
            std::ifstream file(file_path, std::ios::ate | std::ios::binary);

            KIT_ASSERT(LOG_IO, file.is_open(), "File: {} was not opened!", file_path);

            const size_t file_size = static_cast<size_t>(file.tellg());
            std::vector<char> buffer(file_size);

            file.seekg(0);
            file.read(buffer.data(), file_size);

            file.close();

            KIT_ASSERT(LOG_IO, !buffer.empty(), "File: {} could not be properly read!", file_path);

            return buffer;
        }
    };
} // Kitsune
