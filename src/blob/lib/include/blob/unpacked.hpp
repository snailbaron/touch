#pragma once

#include <blob/types.hpp>

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace blob::unpacked {

struct Frame {
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t ms = 0;
};

struct Blob {
    std::vector<uint8_t> sheet;
    std::map<std::string,
        std::map<Speed, std::map<Direction, std::vector<Frame>>>> characters;
    std::map<std::string, std::vector<Frame>> textures;
    std::map<std::string, std::vector<Frame>> objects;
};

} // namespace blob::unpacked
