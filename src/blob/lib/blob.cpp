#include <blob/blob.hpp>

#include <iostream>
#include <stdexcept>
#include <tuple>

namespace blob {

CharacterAnimations::CharacterAnimations(
    const fb::CharacterAnimations* characterAnimations,
    const flatbuffers::Vector<flatbuffers::Offset<fb::Animation>>* animations)
    : _characterAnimations(characterAnimations)
    , _animations(animations)
{ }

Animation CharacterAnimations::move(
    MovementDirection direction, MovementSpeed speed)
{
    const fb::DirectionalAnimationSet* directionalSet = nullptr;
    switch (speed) {
        case MovementSpeed::Stand:
            directionalSet = &_characterAnimations->stand();
            break;
        case MovementSpeed::Walk:
            directionalSet = &_characterAnimations->walk();
            break;
        case MovementSpeed::Run:
            directionalSet = &_characterAnimations->run();
            break;
    }
    if (!directionalSet) {
        throw std::runtime_error{
            "unknown speed: " + std::to_string((int)speed)};
    }

    switch (direction) {
        case MovementDirection::Up:
            return _animations->Get(directionalSet->up());
        case MovementDirection::UpRight:
            return _animations->Get(directionalSet->up_right());
        case MovementDirection::Right:
            return _animations->Get(directionalSet->right());
        case MovementDirection::DownRight:
            return _animations->Get(directionalSet->down_right());
        case MovementDirection::Down:
            return _animations->Get(directionalSet->down());
        case MovementDirection::DownLeft:
            return _animations->Get(directionalSet->down_left());
        case MovementDirection::Left:
            return _animations->Get(directionalSet->left());
        case MovementDirection::UpLeft:
            return _animations->Get(directionalSet->up_left());
    }
    throw std::runtime_error{
        "unknown direction: " + std::to_string((int)direction)};
}

Animation CharacterAnimations::swing(MovementDirection direction)
{
    switch (direction) {
        case MovementDirection::Up:
            return _animations->Get(_characterAnimations->swing().up());
        case MovementDirection::UpRight:
            return _animations->Get(_characterAnimations->swing().up_right());
        case MovementDirection::Right:
            return _animations->Get(_characterAnimations->swing().right());
        case MovementDirection::DownRight:
            return _animations->Get(_characterAnimations->swing().down_right());
        case MovementDirection::Down:
            return _animations->Get(_characterAnimations->swing().down());
        case MovementDirection::DownLeft:
            return _animations->Get(_characterAnimations->swing().down_left());
        case MovementDirection::Left:
            return _animations->Get(_characterAnimations->swing().left());
        case MovementDirection::UpLeft:
            return _animations->Get(_characterAnimations->swing().up_left());
    }
}

Blob::Blob(const std::filesystem::path& blobFile)
    : _map(blobFile)
    , _fb(fb::GetBlob(_map.span().data()))
{ }

std::span<const std::byte> Blob::sheet() const
{
    return {
        reinterpret_cast<const std::byte*>(_fb->sheet()->data()),
        _fb->sheet()->size()
    };
}

CharacterAnimations Blob::heroAnimations() const
{
    return CharacterAnimations{_fb->heroAnimations(), _fb->animations()};
}

} // namespace blob
