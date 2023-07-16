#include <blob/types.hpp>

namespace blob {

std::ostream& operator<<(std::ostream& output, const Direction& direction)
{
    switch (direction) {
        case Direction::Up: return output << "up";
        case Direction::Down: return output << "down";
        case Direction::Left: return output << "left";
        case Direction::Right: return output << "right";
    }
    return output << "<unknown>";
}

std::istream& operator>>(std::istream& input, Direction& direction)
{
    auto s = std::string{};
    input >> s;
    if (s == "up") {
        direction = Direction::Up;
    } else if (s == "down") {
        direction = Direction::Down;
    } else if (s == "left") {
        direction = Direction::Left;
    } else if (s == "right") {
        direction = Direction::Right;
    } else {
        throw std::runtime_error{"unknown direction: " + s};
    }
    return input;
}

std::ostream& operator<<(std::ostream& output, const Speed& speed)
{
    switch (speed) {
        case Speed::Stand: return output << "stand";
        case Speed::Walk: return output << "walk";
        case Speed::Run: return output << "run";
    }
    return output << "<unknown>";
}

std::istream& operator>>(std::istream& input, Speed& speed)
{
    auto s = std::string{};
    input >> s;
    if (s == "stand") {
        speed = Speed::Stand;
    } else if (s == "walk") {
        speed = Speed::Walk;
    } else if (s == "run") {
        speed = Speed::Run;
    } else {
        throw std::runtime_error{"unknown speed: " + s};
    }
    return input;
}

} // namespace blob 
