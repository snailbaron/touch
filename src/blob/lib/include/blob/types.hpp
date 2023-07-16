#pragma once

#include <istream>
#include <ostream>

namespace blob {

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

} // namespace blob
