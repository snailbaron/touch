#pragma once

#include <blob/unpacked.hpp>

#include <filesystem>

namespace blob {

void pack(
    const unpacked::Blob& source, const std::filesystem::path& outputPath);

} // namespace blob
