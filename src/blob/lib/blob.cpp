#include <blob/blob.hpp>

#include <iostream>
#include <tuple>

namespace blob {

namespace {

std::tuple<uint32_t, uint32_t> range(
    const flatbuffers::Vector<uint32_t>* offsets, uint32_t index)
{
    return {offsets->Get(index), offsets->Get(index + 1)};
}

} // namespace

Character::Character(const fb::Blob* fb, uint32_t index)
    : _fb(fb)
    , _index(index)
{ }

std::string_view Character::name() const
{
    auto [begin, end] = range(_fb->name_offsets(), _index);
    return _fb->names()->string_view().substr(begin, end - begin);
}

uint32_t Character::frameCount(Speed speed, Direction direction) const
{
    uint32_t offset = (int)speed * 4 + (int)direction;
    auto [begin, end] = range(_fb->frame_offsets(), _index * 12 + offset);
    return end - begin;
}

const fb::Frame& Character::frame(
    Speed speed, Direction direction, uint32_t frameIndex) const
{
    uint32_t offset = (int)speed * 4 + (int)direction;
    uint32_t startFrame = _fb->frame_offsets()->Get(_index * 12 + offset);
    return *_fb->frames()->Get(startFrame + frameIndex);
}

NamedAnimation::NamedAnimation(const fb::Blob* fb, uint32_t index)
    : _fb(fb)
    , _index(index)
{ }

std::string_view NamedAnimation::name() const
{
    auto [begin, end] = range(_fb->name_offsets(), _index);
    return _fb->names()->string_view().substr(begin, end - begin);
}

uint32_t NamedAnimation::frameCount() const
{
    auto animationIndex = _index + 11 * _fb->character_count();
    auto [begin, end] = range(_fb->frame_offsets(), animationIndex);
    return end - begin;
}

const fb::Frame& NamedAnimation::frame(uint32_t frameIndex) const
{
    auto animationIndex = _index + 11 * _fb->character_count();
    auto startFrame = _fb->frame_offsets()->Get(animationIndex);
    return *_fb->frames()->Get(startFrame + frameIndex);
}

Blob::Blob(const std::filesystem::path& blobFile)
    : _map(blobFile)
    , _fb(fb::GetBlob(_map.span().data()))
{ }

std::span<const std::byte> Blob::sheet() const
{
    std::cout << "sheet size: " << _fb->sheet()->size() << "\n";
    return {
        reinterpret_cast<const std::byte*>(_fb->sheet()->data()),
        _fb->sheet()->size()
    };
}

uint32_t Blob::characterCount() const
{
    return _fb->character_count();
}

Character Blob::character(uint32_t index) const
{
    return Character{_fb, index};
}

uint32_t Blob::textureCount() const
{
    return _fb->texture_count();
}

NamedAnimation Blob::texture(uint32_t index) const
{
    return NamedAnimation{_fb, _fb->character_count() + index};
}

uint32_t Blob::objectCount() const
{
    return _fb->object_count();
}

NamedAnimation Blob::object(uint32_t index) const
{
    return NamedAnimation{
        _fb, _fb->character_count() + _fb->texture_count() + index};
}

} // namespace blob
