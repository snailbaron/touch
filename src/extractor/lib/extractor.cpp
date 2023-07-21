#include <extractor.hpp>

#include <nlohmann/json.hpp>

#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace extractor {

InputManifest InputManifest::read(const std::filesystem::path& inputFile)
{
    auto inputManifest = InputManifest{};

    auto input = std::ifstream{inputFile};
    input.exceptions(std::ios::badbit | std::ios::failbit);
    auto json = nlohmann::json::parse(input);
    for (const auto& path : json["textures"]) {
        inputManifest.textures.push_back(path);
    }
    for (const auto& path : json["objects"]) {
        inputManifest.objects.push_back(path);
    }
    for (const auto& path : json["characters"]) {
        inputManifest.characters.push_back(path);
    }

    return inputManifest;
}

void InputManifest::write(const std::filesystem::path& outputFile)
{
    auto json = nlohmann::json{};
    for (const auto& path : this->textures) {
        json["textures"].push_back(path);
    }
    for (const auto& path : this->objects) {
        json["objects"].push_back(path);
    }
    for (const auto& path : this->characters) {
        json["characters"].push_back(path);
    }

    auto output = std::ofstream{outputFile};
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output << json;
}

SpriteSheetMeta read(const std::filesystem::path& inputFile)
{
    auto input = std::ifstream{inputFile};
    input.exceptions(std::ios::badbit | std::ios::failbit);
    auto json = nlohmann::json::parse(input);

    auto meta = SpriteSheetMeta{};
    meta.name = json["name"].get<std::string>();
    meta.objects.emplace_back();
    for (const auto& jsonObject : json["objects"]) {
        meta.objects.back().name = jsonObject["name"].get<std::string>();
        meta.objects.back().type = jsonObject["type"].get<std::string>();
        for (const auto& jsonFrame : jsonObject["frames"]) {
            meta.objects.back().animations.push_back(Frame{
                .x = jsonFrame["x"].get<int>(),
                .y = jsonFrame["y"].get<int>(),
                .w = jsonFrame["w"].get<int>(),
                .h = jsonFrame["h"].get<int>(),
                .ms = jsonFrame["ms"].get<int>(),
            });
        }
    }

    return meta;
}

void SpriteSheetMeta::write(const std::filesystem::path& outputFile)
{
    auto json = nlohmann::json{};
    json["name"] = this->name;
    for (const auto& object : this->objects) {
        json["objects"].emplace_back();
        json.back()["name"] = object.name;
        json.back()["type"] = string(object.type);
        for (const auto& frame : object.frames) {
            json.back()["frames"].push_back({
                {"x", frame.x},
                {"y", frame.y},
                {"w", frame.w},
                {"h", frame.h},
                {"ms", frame.ms},
            });
        }
    }

    auto output = std::ofstream{outputFile};
    output.exceptions(std::ios::badbit | std::ios::failbit);
    output << std::setw(4) << json;
}

} // namespace extractor
