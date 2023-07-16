#pragma once

#include <blob/unpacked.hpp>

#include <filesystem>

namespace blob {

void pack(
    const std::vector<unpacked::Sheet>& unpackedSheets,
    const std::filesystem::path& outputPath);

} // namespace blob
