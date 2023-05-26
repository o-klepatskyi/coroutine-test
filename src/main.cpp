#include "log.h"
#include "util.h"
#include "FtpExampleCoro.h"
#include "FutureAwaiter.h"
#include "FutureCoro.h"

#include <chrono>
#include <thread>

FutureCoro<void> useCoro(const std::string& path, const std::string& pattern) noexcept
{
    LogInfo("Starting useCoro");
    try
    {
	    kw::FtpExampleCoro ftp;
        LogInfo("Awaiting ftp.downloadFirstMatch");
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
    LogInfo("Starting program");
    const std::string path = getProjectPath() + "/src/include";
    const std::string pattern = ".h";

    auto fut = useCoro(path, pattern);

    while(not fut.ready())
    {
        LogInfo("Doing my job...");
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(10ms);
    }  

    return 0;
}