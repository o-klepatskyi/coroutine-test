#include "FtpExampleCoro.h"
#include "MemoryLeakDetector.h"
#include "util.h"

#include "gtest/gtest.h"
#include <iostream>
#include <chrono>

TEST(FtpWithCoroutine, DownloadsFileAndReturnsName)
{
    MemoryLeakDetector d;
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".h";
    auto predicate = [&pattern](std::string_view f) { return f.ends_with(pattern); };
    kw::FtpExampleCoro ftp;

    auto fileFuture = ftp.downloadFirstMatch(path,
        predicate,
        [](int progress) { LogInfo("Download: %d%%", progress); } );
    
    std::string file = fileFuture.get();
    std::cout << file << "\n";
    ASSERT_TRUE(predicate(file));
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10ms); // wait for memory free
}

TEST(FtpWithCoroutine, ThrowsIfFileIsNotFound)
{
    MemoryLeakDetector d;
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".something";
    kw::FtpExampleCoro ftp;

    auto fileFuture = ftp.downloadFirstMatch(path,
        [&pattern](std::string_view f) { return f.ends_with(pattern); },
        [](int progress) { LogInfo("Download: %d%%", progress); } );

    EXPECT_THROW(fileFuture.get(), std::exception);
}

TEST(FtpWithCoroutine, ThrowsIfDirectoryIsNotFound)
{
    MemoryLeakDetector d;
    const std::string path = getProjectPath() + "/src/notexisting";
    const std::string pattern = ".something";
    kw::FtpExampleCoro ftp;

    auto fileFuture = ftp.downloadFirstMatch(path,
        [&pattern](std::string_view f) { return f.ends_with(pattern); },
        [](int progress) { LogInfo("Download: %d%%", progress); } );

    EXPECT_THROW(fileFuture.get(), std::exception);
}