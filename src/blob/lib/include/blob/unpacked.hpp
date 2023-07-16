#pragma once

#include <cstdint>
#include <filesystem>
#include <istream>
#include <map>
#include <ostream>
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

enum class Direction {
    Up,
    Down,
    Left,
    Right,
};

std::ostream& operator<<(std::ostream& output, const Direction& direction);
std::istream& operator>>(std::istream& input, Direction& direction);

enum class Speed {
    Stand,
    Walk,
    Run,
};

std::ostream& operator<<(std::ostream& output, const Speed& speed);
std::istream& operator>>(std::istream& input, Speed& speed);

struct Sheet {
    std::string name;
    std::vector<uint8_t> data;
    std::map<std::string,
        std::map<Speed, std::map<Direction, std::vector<Frame>>>> characters;
    std::map<std::string, std::vector<Frame>> textures;
    std::map<std::string, std::vector<Frame>> objects;
};

} // namespace blob::unpacked
