#include <arg.hpp>
#include <blob.hpp>
#include <fi.hpp>

#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <ostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>

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
    fs::path input;
    fs::path outputData;
    fs::path outputHeader;
};

void pack(const Paths& paths)
{
    auto tempDir = fs::temp_directory_path();

    auto sheetPath = tempDir / "sheet.png";
    auto metaPath = tempDir / "meta.json";

    std::ostringstream command;
    command << paths.aseprite <<
        " --batch" <<
        " --data " << metaPath <<
        " --format json-array" <<
        " --sheet " << sheetPath <<
        " --sheet-type packed" <<
        " --filename-format '{path}:{title}:{tag}:{frame}'";

    for (const auto& entry : fs::recursive_directory_iterator{paths.input}) {
        if (entry.is_regular_file()) {
            command << " " << entry.path();
        }
    }

    std::cout << "executing command: " << command.str() << "\n";
    std::system(command.str().c_str());

    using namespace blob::unpacked;

    auto blob = blob::unpacked::Blob{};
    blob.sheet = fi::read<uint8_t>(sheetPath);

    auto metaFile = std::ifstream{metaPath};
    auto meta = nlohmann::json::parse(metaFile);
    for (const auto& frame : meta["frames"]) {
        auto filenameString = std::string{frame["filename"]};
        auto [path, name, tag, frameIndex] = split<4>(filenameString, ":");
        auto nameString = std::string{name};
        auto category = fs::canonical(fs::path{path}).filename().string();

        auto frameObject = Frame{
            .x = frame["frame"]["x"].get<uint32_t>(),
            .y = frame["frame"]["y"].get<uint32_t>(),
            .w = frame["frame"]["w"].get<uint32_t>(),
            .h = frame["frame"]["h"].get<uint32_t>(),
            .ms = frame["duration"].get<uint32_t>(),
        };

        if (category == "characters") {
            auto tagStream = std::istringstream{std::string{tag}};
            auto speed = blob::Speed::Stand;
            auto direction = blob::Direction::Up;
            tagStream >> speed >> direction;
            blob.characters[nameString][speed][direction].push_back(frameObject);
        } else if (category == "textures") {
            blob.textures[nameString].push_back(frameObject);
        } else if (category == "objects") {
            blob.objects[nameString].push_back(frameObject);
        } else {
            throw std::runtime_error{"unknown category: " + category};
        }
    }

    blob::pack(blob, paths.outputData);
    blob::generateHeader(blob::Blob{paths.outputData}, paths.outputHeader);
}

enum class Action {
    Pack,
    Unpack,
};

std::ostream& operator<<(std::ostream& output, const Action& action)
{
    switch (action) {
        case Action::Pack: return output << "pack";
        case Action::Unpack: return output << "unpack";
    }
    return output << "<unknown>";
}

std::istream& operator>>(std::istream& input, Action& action)
{
    auto actionString = std::string{};
    input >> actionString;
    auto actionStringLowercase = std::string(actionString.length(), '\0');
    std::ranges::transform(
        actionString,
        actionStringLowercase.begin(),
        [] (char c) { return (char)std::tolower(c); });

    if (actionStringLowercase == "pack") {
        action = Action::Pack;
    } else if (actionStringLowercase == "unpack") {
        action = Action::Unpack;
    } else {
        throw std::runtime_error{"unknown action: '" + actionString + "'"};
    }
    return input;
}

int main(int argc, char* argv[])
{
    auto asepritePath = arg::option<fs::path>()
        .keys("--aseprite")
        .metavar("PATH")
        .help("path to aseprite binary");
    auto input = arg::option<fs::path>()
        .keys("-i", "--input")
        .markRequired()
        .metavar("PATH")
        .help("path to input file or directory");
    auto outputData = arg::option<fs::path>()
        .keys("--output-data")
        .markRequired()
        .metavar("PATH")
        .help("path to output file or directory");
    auto outputHeader = arg::option<fs::path>()
        .keys("--output-header")
        .markRequired()
        .metavar("PATH")
        .help("path to output header");
    auto action = arg::option<Action>()
        .keys("--action")
        .markRequired()
        .metavar("ACTION")
        .help("action to take, 'pack' or 'unpack'");
    arg::helpKeys("-h", "--help");
    arg::parse(argc, argv);

    switch (action) {
        case Action::Pack:
            pack({
                .aseprite = asepritePath,
                .input = input,
                .outputData = outputData,
                .outputHeader = outputHeader,
            });
            break;
        case Action::Unpack:
            // TODO
            break;
    }
}
