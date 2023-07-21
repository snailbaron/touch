#include <extractor.hpp>

#include <arg.hpp>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

template <size_t N>
auto split(std::string_view string, std::string_view separator)
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
    fs::path inputManifest;
    fs::path outputManifest;
    fs::path outputSingleSheet;
};

void extract(const Paths& paths)
{
    auto tempDir = fs::temp_directory_path();
    auto metaPath = tempDir / "meta.json";

    std::ostringstream command;
    command << paths.aseprite <<
        " --batch" <<
        " --data " << metaPath <<
        " --format json-array" <<
        " --sheet " << paths.outputSingleSheet <<
        " --sheet-type packed" <<
        " --filename-format '{path}:{title}:{tag}:{frame}'" <<
        " --inner-padding 1";

    auto inputManifest = extractor::InputManifest::read(paths.inputManifest);
    for (const auto& path : inputManifest.textures) {
        command << path;
    }
    for (const auto& path : inputManifest.objects) {
        command << path;
    }
    for (const auto& path : inputManifest.characters) {
        command << path;
    }

    auto dataMap = std::map<
        std::string,
        std::map<std::string, std::map<
            std::string, std::vector<extractor::Frame>>>>{};

    auto metaFile = std::ifstream{metaPath};
    auto meta = nlohmann::json::parse(metaFile);
    for (const auto& frame : meta["frames"]) {
        auto filenameString = frame["filename"].get<std::string>();
        auto [path, objectName, animationName, frameIndex] =
            split<4>(filenameString, ":");
        auto category = fs::canonical(fs::path{path}).filename().string();

        dataMap[category][std::string{objectName}][std::string{animationName}]
            .push_back(extractor::Frame{
                .x = frame["frame"]["x"].get<int>(),
                .y = frame["frame"]["y"].get<int>(),
                .w = frame["frame"]["w"].get<int>(),
                .h = frame["frame"]["h"].get<int>(),
                .ms = frame["duration"].get<int>(),
            });
    }

    auto sheet = extractor::SpriteSheetMeta{};
    sheet.name = "base";
    for (const auto& [objectType, info1] : dataMap) {
        for (const auto& [objectName, info2] : info1) {
            auto& object = sheet.objects.emplace_back();
            object.name = objectName;
            object.type = objectType;
            for (const auto& [animationName, frames] : info2) {
                auto& animation = object.animations.emplace_back();
                animation.name = animationName;
                for (const auto& frame : frames) {
                    animation.frames.push_back(frame);
                }
            }
        }
    }

    sheet.write(paths.outputManifest);
}

int main(int argc, char* argv[])
{
    arg::helpKeys("-h", "--help");
    auto asepritePath = arg::option<fs::path>()
        .keys("--aseprite")
        .metavar("FILE")
        .help("path to aseprite binary");
    auto inputManifestPath = arg::option<fs::path>()
        .keys("--input-manifest")
        .metavar("FILE")
        .help("path to input manifest file");
    auto outputManifestPath = arg::option<fs::path>()
        .keys("--output-manifest")
        .metavar("FILE")
        .help("path to output manifest file");
    auto outputSingleSheetPath = arg::option<fs::path>()
        .keys("--output-single-sheet")
        .metavar("FILE")
        .help("path to combined sprite sheet");
    arg::parse(argc, argv);

    extract({
        .aseprite = asepritePath,
        .inputManifest = inputManifestPath,
        .outputManifest = outputManifestPath,
        .outputSingleSheet = outputSingleSheetPath,
    });
}
