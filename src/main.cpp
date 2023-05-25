#include "log.h"
#include "util.h"
#include "FtpExampleCoro.h"
#include "FutureAwaiter.h"
#include "FutureCoro.h"

FutureCoro<void> useCoro(const std::string& path, const std::string& pattern) noexcept
{
    LogThreadId();
    try
    {
	    kw::FtpExampleCoro ftp;

	    auto file = co_await ftp.downloadFirstMatch(path,
			[&pattern](std::string_view f) { return f.ends_with(pattern); },
	        [](int progress) { LogInfo("Download: %d%%", progress); });
        
        LogInfo("downloadFirstMatch success: %s", file.c_str());
    }
    catch (const std::exception& e)
    {
        LogError("downloadFirstMatch failed: %s", e.what());
    }
    co_return;
}

int main(int, char**)
{
    LogThreadId();
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".h";
    useCoro(path, pattern).wait();

    return 0;
}