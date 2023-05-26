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

        RemoteDirEntry(const RemoteDirEntry& other) = default;
        RemoteDirEntry& operator=(const RemoteDirEntry& other) = default;
        RemoteDirEntry(RemoteDirEntry&& other) = default;
        RemoteDirEntry& operator=(RemoteDirEntry&& other) = default;

        const char* path() const noexcept { return remotePath.c_str(); }
    };
}