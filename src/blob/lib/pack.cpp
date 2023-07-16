#include <blob/pack.hpp>

#include <blob/format_generated.h>

#include <fi.hpp>

#include <algorithm>
#include <sstream>

namespace blob {

namespace {

template <class T>
void append(std::vector<T>& lhs, const std::vector<T>& rhs)
{
    std::copy(rhs.begin(), rhs.end(), std::back_inserter(lhs));
}

void append(std::vector<char>& lhs, const std::string& rhs)
{
    std::copy(rhs.begin(), rhs.end(), std::back_inserter(lhs));
}

} // namespace

void pack(const unpacked::Blob& source, const std::filesystem::path& outputPath)
{
    using namespace unpacked;

    auto builder = flatbuffers::FlatBufferBuilder{};

    auto names = std::vector<char>{};
    auto nameOffsets = std::vector<uint32_t>{};
    auto frames = std::vector<fb::Frame>{};
    auto frameOffsets = std::vector<uint32_t>{};
    uint32_t characterCount = 0;
    uint32_t textureCount = 0;
    uint32_t objectCount = 0;

    for (const auto& [characterName, characterAnimations] : source.characters) {
        nameOffsets.push_back(names.size());
        append(names, characterName);
        for (auto speed : {Speed::Stand, Speed::Walk, Speed::Run}) {
            for (auto direction : {
                    Direction::Up,
                    Direction::Down,
                    Direction::Left,
                    Direction::Right}) {
                if (!characterAnimations.contains(speed) ||
                        !characterAnimations.at(speed).contains(direction)) {
                    auto error = std::ostringstream{};
                    error << "character '" << characterName <<
                        "' does not have animation for '" <<
                        speed << " " << direction << "'";
                    throw std::runtime_error{error.str()};
                }

                const auto& sourceFrames =
                    characterAnimations.at(speed).at(direction);
                frameOffsets.push_back(frames.size());
                for (const auto& f : sourceFrames) {
                    frames.emplace_back(f.x, f.y, f.w, f.h, f.ms);
                }
            }
        }
    }
    characterCount = nameOffsets.size();

    for (const auto& [textureName, textureAnimation] : source.textures) {
        nameOffsets.push_back(names.size());
        append(names, textureName);

        frameOffsets.push_back(frames.size());
        for (const auto& f : textureAnimation) {
            frames.emplace_back(f.x, f.y, f.w, f.h, f.ms);
        }
    }
    textureCount = nameOffsets.size() - characterCount;

    for (const auto& [objectName, objectAnimation] : source.objects) {
        nameOffsets.push_back(names.size());
        append(names, objectName);

        frameOffsets.push_back(frames.size());
        for (const auto& f : objectAnimation) {
            frames.emplace_back(f.x, f.y, f.w, f.h, f.ms);
        }
    }
    objectCount = nameOffsets.size() - textureCount - characterCount;

    auto fbBlob = fb::CreateBlob(
        builder,
        builder.CreateVector(source.sheet),
        builder.CreateString(names.data(), names.size()),
        builder.CreateVector(nameOffsets),
        builder.CreateVectorOfStructs(frames),
        builder.CreateVector(frameOffsets),
        characterCount,
        textureCount,
        objectCount);

    builder.Finish(fbBlob);

    fi::write<unsigned char>(outputPath, builder.GetBufferSpan());
}

} // namespace blob
