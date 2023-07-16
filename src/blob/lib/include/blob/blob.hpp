#pragma once

#include <blob/format_generated.h>
#include <blob/types.hpp>

#include <fi.hpp>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <map>
#include <span>
#include <string_view>

namespace blob {

class Character {
public:
    std::string_view name() const;
    std::span<const fb::Frame* const> animation(
        Speed speed, Direction direction);

private:
    Character(const fb::Blob* fb, uint32_t index);

    const fb::Blob* _fb = nullptr;
    uint32_t _index = 0;

    friend class Blob;
};

class NamedAnimation {
public:
    std::string_view name() const;
    std::span<const fb::Frame* const> animation() const;

private:
    NamedAnimation(const fb::Blob* fb, uint32_t index);

    const fb::Blob* _fb = nullptr;
    uint32_t _index = 0;

    friend class Blob;
};

class Blob {
public:
    explicit Blob(const std::filesystem::path& blobFile);

    std::span<const std::byte> sheet() const;

    uint32_t characterCount() const;
    Character character(uint32_t index) const;

    uint32_t textureCount() const;
    NamedAnimation texture(uint32_t index) const;

    uint32_t objectCount() const;
    NamedAnimation object(uint32_t index) const;

private:
    fi::MemoryMappedFile _map;
    const fb::Blob* _fb;
};

} // namespace blob
