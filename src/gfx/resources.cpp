#include <gfx/resources.hpp>

#include <fi.hpp>

#include <iostream>
#include <span>

namespace gfx {

Animation::Animation(const sdl::Texture& texture, std::vector<SDL_Rect> frames)
    : _texture(texture)
    , _frames(std::move(frames))
{ }

const sdl::Texture& Animation::texture() const
{
    return _texture;
}

size_t Animation::frameCount() const
{
    return _frames.size();
}

const SDL_Rect& Animation::frame(size_t index) const
{
    return _frames.at(index);
}

CharacterAnimations::CharacterAnimations(std::vector<Animation> animations)
    : _animations(std::move(animations))
{ }

Animation CharacterAnimations::animation(Speed speed, Direction direction)
{
    return _animations.at((int)speed * 4 + (int)direction);
}

Resources::Resources(const sdl::Renderer& renderer)
    : _blob(fi::BUILD_ROOT / "data.blob")
    , _texture(renderer.loadTexture(_blob.sheet()))
{
    std::cout << "resource data loaded from " <<
        (fi::BUILD_ROOT / "data.blob") << "\n";
}

CharacterAnimations Resources::operator[](r::Character characterId) const
{
    using blob::Speed;
    using blob::Direction;

    auto data = _blob.character((uint32_t)characterId);
    auto animations = std::vector<Animation>{};
    for (auto speed : {Speed::Stand, Speed::Walk, Speed::Run}) {
        for (auto direction : {
                Direction::Up,
                Direction::Down,
                Direction::Left,
                Direction::Right}) {
            auto convertedFrames = std::vector<SDL_Rect>{};
            for (size_t i = 0; i < data.frameCount(speed, direction); i++) {
                const auto& f = data.frame(speed, direction, i);
                convertedFrames.push_back({
                    .x = (int)f.x(),
                    .y = (int)f.y(),
                    .w = (int)f.w(),
                    .h = (int)f.h()
                });
            }
            animations.push_back(
                Animation{_texture, std::move(convertedFrames)});
        }
    }

    return CharacterAnimations{std::move(animations)};
}

Animation Resources::operator[](r::Texture textureId) const
{
    auto data = _blob.texture((uint32_t)textureId);
    auto convertedFrames = std::vector<SDL_Rect>{};
    for (size_t i = 0; i < data.frameCount(); i++) {
        const auto& f = data.frame(i);
        convertedFrames.push_back({
            .x = (int)f.x(), .y = (int)f.y(), .w = (int)f.w(), .h = (int)f.h()
        });
    }
    return Animation{_texture, std::move(convertedFrames)};
}

Animation Resources::operator[](r::Object objectId) const
{
    auto data = _blob.object((uint32_t)objectId);
    auto convertedFrames = std::vector<SDL_Rect>{};
    for (size_t i = 0; i < data.frameCount(); i++) {
        const auto& f = data.frame(i);
        convertedFrames.push_back({
            .x = (int)f.x(), .y = (int)f.y(), .w = (int)f.w(), .h = (int)f.h()
        });
    }
    return Animation{_texture, std::move(convertedFrames)};
}

} // namespace gfx
