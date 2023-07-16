#include <blob/pack.hpp>

#include "format_generated.h"

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

} // namespace

void pack(
    const std::vector<unpacked::Sheet>& unpackedSheets,
    const std::filesystem::path& outputPath)
{
    using namespace unpacked;

    auto builder = flatbuffers::FlatBufferBuilder{};

    auto stringData = std::string{};
    auto sheetData = std::vector<uint8_t>{};
    auto sheets = std::vector<fb::Sheet>{};
    auto frames = std::vector<fb::Frame>{};
    auto animations = std::vector<fb::Animation>{};
    auto characters = std::vector<fb::NamedAnimation>{};
    auto textures = std::vector<fb::NamedAnimation>{};
    auto objects = std::vector<fb::NamedAnimation>{};

    for (size_t sheetIndex = 0; sheetIndex < unpackedSheets.size();
            sheetIndex++) {
        const auto& unpackedSheet = unpackedSheets.at(sheetIndex);
        sheets.emplace_back(
            stringData.size(),
            stringData.size() + unpackedSheet.name.size(),
            sheetData.size(),
            sheetData.size() + unpackedSheet.data.size());
        stringData += unpackedSheet.name;
        append(sheetData, unpackedSheet.data);

        for (const auto& [characterName, characterAnimations] :
                unpackedSheet.characters) {
            characters.emplace_back(
                stringData.size(),
                stringData.size() + characterName.size(),
                animations.size());
            stringData += characterName;
            for (auto speed : {Speed::Stand, Speed::Walk, Speed::Run}) {
                for (auto direction : {
                        Direction::Up,
                        Direction::Down,
                        Direction::Left,
                        Direction::Right}) {
                    const auto& animation =
                        characterAnimations.at(speed).at(direction);
                    animations.emplace_back(
                        sheetIndex,
                        frames.size(),
                        frames.size() + animation.size());
                    for (const auto& frame : animation) {
                        frames.emplace_back(
                            frame.x, frame.y, frame.w, frame.h, frame.ms);
                    }
                }
            }
        }

        for (const auto& [textureName, textureAnimation] :
                unpackedSheet.textures) {
            textures.emplace_back(
                stringData.size(),
                stringData.size() + textureName.size(),
                animations.size());
            stringData += textureName;
            animations.emplace_back(
                sheetIndex,
                frames.size(),
                frames.size() + textureAnimation.size());
            for (const auto& frame : textureAnimation) {
                frames.emplace_back(
                    frame.x, frame.y, frame.w, frame.h, frame.ms);
            }
        }

        for (const auto& [objectName, objectAnimation] :
                unpackedSheet.objects) {
            objects.emplace_back(
                stringData.size(),
                stringData.size() + objectName.size(),
                animations.size());
            stringData += objectName;
            animations.emplace_back(
                sheetIndex,
                frames.size(),
                frames.size() + objectAnimation.size());
            for (const auto& frame : objectAnimation) {
                frames.emplace_back(
                    frame.x, frame.y, frame.w, frame.h, frame.ms);
            }
        }
    }

    auto fbBlob = fb::CreateBlob(
        builder,
        builder.CreateString(stringData),
        builder.CreateVector(sheetData),
        builder.CreateVectorOfStructs(sheets),
        builder.CreateVectorOfStructs(frames),
        builder.CreateVectorOfStructs(animations),
        builder.CreateVectorOfStructs(characters),
        builder.CreateVectorOfStructs(textures),
        builder.CreateVectorOfStructs(objects));

    builder.Finish(fbBlob);

    fi::write<unsigned char>(outputPath, builder.GetBufferSpan());
}

} // namespace blob
