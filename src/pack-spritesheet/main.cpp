#include <arg.hpp>

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class FakeLogger {};
FakeLogger fakeLogger;

FakeLogger operator<<(FakeLogger&, const auto&&) { return {}; }

template <size_t N>
auto split(const std::string& string, std::string_view separator)
{
    if constexpr (N == 0) {
        return std::tuple{};
    } else {
        auto p = string.find(separator);
        if (N != 1 && p == std::string_view::npos) {
            throw std::runtime_error{
                "cannot split into " + std::to_string(N) +
                " elements: " + std::string{string}
            };
        }
        return std::tuple_cat(
            std::tuple{string.substr(0, p)},
            split<N - 1>(string.substr(p + 1), separator));
    }
}

struct Paths {
    fs::path aseprite;
    fs::path root;
    fs::path manifest;
    fs::path outputSheet;
    fs::path outputMeta;
};

void pack(const Paths& paths)
{
    auto inputFilePaths = std::vector<fs::path>{};

    auto manifest = YAML::LoadFile(paths.manifest);
    for (const auto& object : manifest) {
        auto relativePath = fs::path{object["path"].as<std::string>()};
        inputFilePaths.emplace_back(paths.root / relativePath);
    }

    auto tempDir = fs::temp_directory_path();
    auto asepriteMetaPath = tempDir / "meta.json";

    auto command = std::ostringstream{};
    command << paths.aseprite <<
        " --batch" <<
        " --data " << asepriteMetaPath <<
        " --format json-array" <<
        " --sheet " << paths.outputSheet <<
        " --sheet-type packed" <<
        " --filename-format '{path}:{title}:{tag}:{tagframe}'" <<
        " --inner-padding 1";
    for (const auto& inputFilePath : inputFilePaths) {
        command << " " << inputFilePath;
    }

    std::cerr << "writing packed spritesheet to " << paths.outputSheet << "\n";
    std::cerr << "writing aseprite metainfo to " << asepriteMetaPath << "\n";
    std::system(command.str().c_str());

    struct Frame {
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;
        int ms = 0;
    };

    auto animationMap =
        std::map<std::string, std::map<std::string, YAML::Node>>{};

    auto asepriteMeta = YAML::LoadFile(asepriteMetaPath);
    for (const auto& frameInfo : asepriteMeta["frames"]) {
        auto filenameString = frameInfo["filename"].as<std::string>();
        auto [path, objectName, animationName, frameIndexString] =
            split<4>(filenameString, ":");
        auto frame = frameInfo["frame"];
        auto frameIndex = std::stol(frameIndexString);

        auto frameNode = YAML::Node{};
        frameNode["x"] = frame["x"].as<int>();
        frameNode["y"] = frame["y"].as<int>();
        frameNode["w"] = frame["w"].as<int>();
        frameNode["h"] = frame["h"].as<int>();
        frameNode["ms"] = frameInfo["duration"].as<int>();
        animationMap[objectName][animationName][frameIndex] = frameNode;
    }

    auto yaml = YAML::Emitter{};
    yaml << YAML::BeginSeq;
    for (const auto& [objectName, objectInfo] : animationMap) {
        yaml << YAML::BeginMap;
        yaml << YAML::Key << "name" << YAML::Value << objectName;
        yaml << YAML::Key << "animations" << YAML::Value << YAML::BeginSeq;
        for (const auto& [animationName, animationFrames] : objectInfo) {
            yaml << YAML::BeginMap;
            yaml << YAML::Key << "name" << YAML::Value << animationName;
            yaml << YAML::Key << "frames" << YAML::Value << YAML::BeginSeq;
            for (const auto& animationFrame : animationFrames) {
                yaml << YAML::Flow << animationFrame;
            }
            yaml << YAML::EndSeq;
            yaml << YAML::EndMap;
        }
        yaml << YAML::EndSeq;
        yaml << YAML::EndMap;
    }
    yaml << YAML::EndSeq;

    std::cout << "writing meta info to " << paths.outputMeta << "\n";
    auto output = std::ofstream{paths.outputMeta};
    output << yaml.c_str();
}

int main(int argc, char* argv[])
{
    arg::helpKeys("-h", "--help");
    auto asepritePath = arg::option<fs::path>()
        .keys("--aseprite")
        .metavar("FILE")
        .help("path to aseprite binary");
    auto rootPath = arg::option<fs::path>()
        .keys("--root")
        .metavar("DIR")
        .help("root directory with assets to pack");
    auto manifestPath = arg::option<fs::path>()
        .keys("--manifest")
        .metavar("FILE")
        .help("path to manifest file");
    auto outputSheetPath = arg::option<fs::path>()
        .keys("--output-sheet")
        .metavar("FILE")
        .help("path to output spritesheet file");
    auto outputMetaPath = arg::option<fs::path>()
        .keys("--output-meta")
        .metavar("FILE")
        .help("path to output metadata file");
    arg::parse(argc, argv);

    std::cout << "packing spritesheet:\n" <<
        "    aseprite path      : " << asepritePath << "\n" <<
        "    root path          : " << rootPath << "\n" <<
        "    manifest path      : " << manifestPath << "\n" <<
        "    output spritesheet : " << outputSheetPath << "\n" <<
        "    output metadata    : " << outputMetaPath << "\n";
    std::cout << "current working directory: " << fs::current_path() << "\n";

    pack(Paths{
        .aseprite = asepritePath,
        .root = rootPath,
        .manifest = manifestPath,
        .outputSheet = outputSheetPath,
        .outputMeta = outputMetaPath,
    });
}
