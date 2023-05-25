#pragma once
#include <filesystem>
#include <string>

namespace kw {
    namespace fs = std::filesystem;

    struct RemoteDirEntry
    {
        std::string remotePath;
        size_t size = 0;
        bool isFile = false; // assume remote only has regular files or dirs

        RemoteDirEntry() noexcept = default;
        RemoteDirEntry(std::string path, size_t size, bool isFile) noexcept
            : remotePath{path}, size{size}, isFile{isFile} {}

        const char* path() const noexcept { return remotePath.c_str(); }
    };
}