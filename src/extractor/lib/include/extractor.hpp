#pragma once

#include <filesystem>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace extractor {

struct InputManifest {
    static InputManifest read(const std::filesystem::path& inputFile);
    void write(const std::filesystem::path& outputFile);

    std::vector<std::filesystem::path> textures;
    std::vector<std::filesystem::path> objects;
    std::vector<std::filesystem::path> characters;
};

struct Frame {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
    int ms = 0;
};

struct Animation {
    std::string name;
    std::vector<Frame> frames;
};

struct Object {
    std::string name;
    std::string type;
    std::vector<Animation> animations;
};

struct SpriteSheetMeta {
    static SpriteSheetMeta read(const std::filesystem::path& inputFile);
    void write(const std::filesystem::path& outputFile);

    std::string name;
    std::vector<Object> objects;
};

} // namespace extractor
