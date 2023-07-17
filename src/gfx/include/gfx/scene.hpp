#pragma once

#include <gfx/resources.hpp>
#include <gfx/sdl-wrapper.hpp>

#include <tempo.hpp>
#include <ve.hpp>

#include <deque>
#include <optional>
#include <vector>

namespace gfx {

using Speed = blob::Speed;
using Direction = blob::Direction;

template <class T> struct XYModel {
    T x;
    T y;
};
using Position = ve::Point<XYModel, float>;
using Vector = ve::Vector<XYModel, float>;

struct Camera {
    Position toScreen(const Position& worldPosition) const;

    Position position;
    Vector screenSize;
    float meterSize = 32.f;
    float zoom = 3.f;
};

class Sprite {
public:
    void update(double delta);
    void render(sdl::Renderer& renderer, Position screenPosition, float zoom);

    Position position;

private:
    Sprite(Animation animation, const Position& position);

    tempo::Metronome _metronome = tempo::Metronome{3};
    Animation _animation;
    size_t _frame = 0;

    friend class Scene;
};

class CharacterSprite {
public:
    void update(double delta);
    void render(sdl::Renderer& renderer, Position screenPosition, float zoom);

    Position position;
    Speed speed = Speed::Stand;
    Direction direction = Direction::Down;

private:
    CharacterSprite(CharacterAnimations animations, const Position& position);

    tempo::Metronome _metronome = tempo::Metronome{3};
    CharacterAnimations _animations;
    size_t _frame = 0;

    friend class Scene;
};

class Scene {
public:
    Scene(const Resources& resources);

    size_t add(r::Object objectId, const Position& position);
    size_t add(r::Character characterId, const Position& position);

    void removeObject(size_t objectId);

    Camera& camera();
    Sprite& sprite(size_t index);
    CharacterSprite& character(size_t index);

    void setScreenSize(int w, int h);

    void update(double delta);
    void render(sdl::Renderer& renderer);

private:
    const Resources& _resources;
    Camera _camera;
    std::vector<std::optional<Sprite>> _sprites;
    std::vector<CharacterSprite> _characters;
    std::deque<size_t> _freeSprites;
    std::deque<size_t> _freeCharacters;
};

} // namespace gfx
