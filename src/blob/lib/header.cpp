#include <blob/header.hpp>

#include <blob/blob.hpp>

#include <cstdlib>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace blob {

namespace {

std::string toEnumName(std::string_view source)
{
    auto s = std::string{source};
    if (!s.empty()) {
        s[0] = (char)std::toupper(s[0]);
    }
    return s;
}

} // namespace

void generateHeader(
    const Blob& blob, const fs::path& outputHeaderPath)
{
    auto output = std::ofstream{outputHeaderPath};
    output.exceptions(std::ios::badbit | std::ios::failbit);

    output <<
        "#include <cstdint>\n" <<
        "\n" <<
        "namespace r {\n" <<
        "\n" <<
        "enum class Character : uint32_t {\n";
    for (size_t i = 0; i < blob.characterCount(); i++) {
        output << "    " << toEnumName(blob.character(i).name()) <<
            " = " << i << ",\n";
    }
    output << "};\n\nenum class Texture : uint32_t {\n";
    for (size_t i = 0; i < blob.textureCount(); i++) {
        output << "    " << toEnumName(blob.texture(i).name()) <<
            " = " << i << ",\n";
    }
    output << "};\n\nenum class Object : uint32_t {\n";
    for (size_t i = 0; i < blob.objectCount(); i++) {
        output << "    " << toEnumName(blob.object(i).name()) <<
            " = " << i << ",\n";
    }
    output << "};\n\n} // namespace r\n";
}

} // namespace blob
