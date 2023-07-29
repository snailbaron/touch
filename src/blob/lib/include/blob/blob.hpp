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

template <class T>
class VectorIterator {
public:
    explicit VectorIterator(const T** ptr)
        : _ptr(ptr)
    { }

    VectorIterator& operator++()
    {
        ++_ptr;
        return *this;
    }

    VectorIterator operator++(int)
    {
        auto old = *this;
        ++_ptr;
        return old;
    }

    VectorIterator& operator--()
    {
        --_ptr;
        return *this;
    }

    VectorIterator operator--(int)
    {
        auto old = *this;
        --_ptr;
        return old;
    }

    T operator*() const
    {
        return *_ptr;
    }

private:
    const T** _ptr = nullptr;
};

template <class T>
class VectorRange {
public:
    VectorRange(const flatbuffers::Vector<const T*>* fbVectorPtr)
        : _fbVectorPtr(fbVectorPtr)
    { }

    VectorIterator<T> begin() const
    {
        return {_fbVectorPtr->data()};
    }

    VectorIterator<T> end() const
    {
        return {_fbVectorPtr->data() + _fbVectorPtr->size()};
    }

    size_t size() const
    {
        return _fbVectorPtr->size();
    }

private:
    const flatbuffers::Vector<const T*>* _fbVectorPtr = nullptr;
};

using Frame = fb::Frame;
using MovementDirection = fb::MovementDirection;
using MovementSpeed = fb::MovementSpeed;
using ObjectType = fb::ObjectType;
using Portal = fb::Portal;
using Position = fb::Position;
using SimpleObject = fb::SimpleObject;
using Turret = fb::Turret;

class Animation {
public:
    constexpr Animation(const fb::Animation* fb) : _fb(fb) {}

    std::string_view name() const { return _fb->name()->string_view(); }
    VectorRange<fb::Frame> frames() const { return _fb->frames(); }

private:
    const fb::Animation* _fb = nullptr;
};

class CharacterAnimations {
public:
    Animation move(MovementDirection direction, MovementSpeed speed);
    Animation swing(MovementDirection direction);

private:
    explicit CharacterAnimations(
        const fb::CharacterAnimations* characterAnimations,
        const flatbuffers::Vector<
            flatbuffers::Offset<fb::Animation>>* animations);

    const fb::CharacterAnimations* _characterAnimations = nullptr;
    const flatbuffers::Vector<
        flatbuffers::Offset<fb::Animation>>* _animations = nullptr;

    friend class Blob;
};

class Blob {
public:
    explicit Blob(const std::filesystem::path& blobFile);

    std::span<const std::byte> sheet() const;
    CharacterAnimations heroAnimations() const;

private:
    fi::MemoryMappedFile _map;
    const fb::Blob* _fb;
};

} // namespace blob
