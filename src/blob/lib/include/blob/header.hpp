#pragma once

#include <blob/blob.hpp>

#include <filesystem>

namespace blob {

void generateHeader(
    const Blob& blobFilePath,
    const std::filesystem::path& outputHeaderPath);

} // namespace blob
