#include <gfx/scene.hpp>

#include <iostream>

namespace gfx {

Position Camera::toScreen(const Position& worldPosition) const
{
    auto screenMiddle = screenSize / 2;
    auto offset = worldPosition - position;

    return {
        screenMiddle.x + offset.x * meterSize * zoom,
        screenMiddle.y - offset.y * meterSize * zoom,
    };
}

Sprite::Sprite(Animation animation, const Position& position)
    : position(position)
    , _animation(std::move(animation))
{ }

void Sprite::update(double delta)
{
    _frame = (_frame + _metronome.ticks(delta)) % _animation.frameCount();
}

void Sprite::render(
    sdl::Renderer& renderer, Position screenPosition, float zoom)
{
    auto frame = _animation.frame(_frame);

    auto target = SDL_FRect{
        .x = screenPosition.x - (float)frame.w * zoom / 2,
        .y = screenPosition.y - (float)frame.h * zoom / 2,
        .w = (float)frame.w * zoom,
        .h = (float)frame.h * zoom,
    };

    renderer.copy(_animation.texture(), frame, target);
}

CharacterSprite::CharacterSprite(
    CharacterAnimations animations, const Position& position)
    : position(position)
    , _animations(std::move(animations))
{ }

void CharacterSprite::update(double delta)
{
    _frame += _metronome.ticks(delta);
}

void CharacterSprite::render(
    sdl::Renderer& renderer, Position screenPosition, float zoom)
{
    const auto& animation = _animations.animation(speed, direction);
    auto frame = animation.frame(_frame % animation.frameCount());

    auto target = SDL_FRect{
        .x = screenPosition.x - (float)frame.w * zoom / 2,
        .y = screenPosition.y - (float)frame.h * zoom / 2,
        .w = (float)frame.w * zoom,
        .h = (float)frame.h * zoom,
    };

    renderer.copy(animation.texture(), frame, target);
}

Scene::Scene(const Resources& resources)
    : _resources(resources)
{ }

size_t Scene::add(r::Object objectId, const Position& position)
{
    _sprites.push_back(Sprite{_resources[objectId], position});
    return _sprites.size() - 1;
}

size_t Scene::add(r::Character characterId, const Position& position)
{
    _characters.push_back(CharacterSprite{_resources[characterId], position});
    return _characters.size() - 1;
}

Sprite& Scene::sprite(size_t index)
{
    return _sprites.at(index);
}

CharacterSprite& Scene::character(size_t index)
{
    return _characters.at(index);
}

void Scene::setScreenSize(int w, int h)
{
    _camera.screenSize = {(float)w, (float)h};
}

void Scene::update(double delta)
{
    for (auto& sprite : _sprites) {
        sprite.update(delta);
    }
    for (auto& character : _characters) {
        character.update(delta);
    }
}

void Scene::render(sdl::Renderer& renderer)
{
    for (auto& sprite : _sprites) {
        auto screenPosition = _camera.toScreen(sprite.position);
        sprite.render(renderer, screenPosition, _camera.zoom);
    }

    for (auto& character : _characters) {
        auto screenPosition = _camera.toScreen(character.position);
        character.render(renderer, screenPosition, _camera.zoom);
    }
}

} // namespace gfx
