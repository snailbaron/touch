#pragma once

#include <gfx/resource_ids.hpp>
#include <gfx/sdl-wrapper.hpp>

#include <blob.hpp>

#include <filesystem>
#include <span>
#include <vector>

namespace gfx {

struct Animation {
public:
    const sdl::Texture& texture() const;

    size_t frameCount() const;
    const SDL_Rect& frame(size_t index) const;

private:
    Animation(const sdl::Texture& texture, std::vector<SDL_Rect> frames);

    const sdl::Texture* _texture;
    std::vector<SDL_Rect> _frames;

    friend class Resources;
};

class CharacterAnimations {
public:
    using Speed = blob::Speed;
    using Direction = blob::Direction;

    Animation animation(Speed speed, Direction direction);

private:
    explicit CharacterAnimations(std::vector<Animation> animations);

    std::vector<Animation> _animations;

    friend class Resources;
};

class Resources {
public:
    Resources(const sdl::Renderer& renderer);

    CharacterAnimations operator[](r::Character characterId) const;
    Animation operator[](r::Texture textureId) const;
    Animation operator[](r::Object objectId) const;

private:
    blob::Blob _blob;
    sdl::Texture _texture;
};

} // namespace gfx
