#include "FtpExampleCoro.h"
#include "MemoryLeakDetector.h"
#include "util.h"

#include "gtest/gtest.h"

TEST(FtpWithCoroutine, DownloadsFileAndReturnsName)
{
    MemoryLeakDetector d;
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".h";
    kw::FtpExampleCoro ftp;

    auto fileFuture = ftp.downloadFirstMatch(path,
        [&pattern](std::string_view f) { return f.ends_with(pattern); },
        [](int){} );

    EXPECT_FALSE(fileFuture.get().empty());
}

TEST(FtpWithCoroutine, ThrowsIfFileIsNotFound)
{
    MemoryLeakDetector d;
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".something";
    kw::FtpExampleCoro ftp;

    auto fileFuture = ftp.downloadFirstMatch(path,
        [&pattern](std::string_view f) { return f.ends_with(pattern); },
        [](int){} );

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
        [](int){} );

    EXPECT_THROW(fileFuture.get(), std::exception);
}