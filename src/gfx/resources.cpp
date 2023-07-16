#include <gfx/resources.hpp>

#include <fi.hpp>

#include <span>

namespace gfx {

namespace {

std::vector<SDL_Rect> convertFrames(
    std::span<const blob::fb::Frame* const> source)
{
    auto frames = std::vector<SDL_Rect>{};
    for (auto* f : source) {
        frames.push_back({
            .x = (int)f->x(),
            .y = (int)f->y(),
            .w = (int)f->w(),
            .h = (int)f->h()
        });
    }
    return frames;
}

} // namespace

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

Animation CharacterAnimations::animation(Speed speed, Direction direction)
{
    return _animations.at((int)speed * 4 + (int)direction);
}

Resources::Resources(const sdl::Renderer& renderer)
    : _blob(fi::BUILD_ROOT / "data.blob")
    , _texture(renderer.loadTexture(_blob.sheet()))
{ }

CharacterAnimations::CharacterAnimations(std::vector<Animation> animations)
    : _animations(std::move(animations))
{ }

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
            auto sourceFrames = data.animation(speed, direction);
            animations.push_back(
                Animation{_texture, convertFrames(sourceFrames)});
        }
    }

    return CharacterAnimations{std::move(animations)};
}

Animation Resources::operator[](r::Texture textureId) const
{
    auto data = _blob.texture((uint32_t)textureId);
    return Animation{_texture, convertFrames(data.animation())};
}

Animation Resources::operator[](r::Object objectId) const
{
    auto data = _blob.object((uint32_t)objectId);
    return Animation{_texture, convertFrames(data.animation())};
}

} // namespace gfx
