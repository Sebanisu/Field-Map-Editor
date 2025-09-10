//
// Created by pcvii on 3/1/2023.
//

#include "open_file_explorer.hpp"
#include "safedir.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <CoreServices/CoreServices.h>
#elif defined(__linux__)
#include <cstdlib>
#endif
void open_directory(const std::filesystem::path &path)
{
#if defined(_WIN32)
     std::wstring wide_path = path.wstring();
     ShellExecuteW(nullptr, L"open", wide_path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__)
     CFStringRef     cf_path     = CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8);
     LSLaunchURLSpec launch_spec = { 0 };
     launch_spec.appURL          = nullptr;
     launch_spec.itemURLs        = (CFArrayRef)CFBridgingRetain(@[ (__bridge id)(cf_path) ]);
     launch_spec.launchFlags     = kLSLaunchDefaults | kLSLaunchAsync;
     OSStatus status             = LSOpenFromURLSpec(&launch_spec, nullptr);
     CFRelease(cf_path);
     CFRelease(launch_spec.itemURLs);
     if (status != noErr)
     {
          // handle error
     }
#elif defined(__linux__)
     // not thread safe.
     const std::string cmd = "xdg-open " + path.string();
     const int         ret = std::system(cmd.c_str());
     if (ret != 0)
     {
          spdlog::error("Command execution failed with error code {}. {}", ret, strerror(errno));
     }
#endif
}
void open_file_explorer(const std::filesystem::path &path)
{
     const auto safe = safedir{ path };
     if (!safe.is_exists())
     {
          return;
     }
     if (safe.is_dir())
     {
          open_directory(path);
          return;
     }
#if defined(_WIN32)
     //     std::wstring wide_path = path.wstring();
     //     ShellExecuteW(nullptr, L"open", wide_path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
     std::wstring wide_path     = path.wstring();
     std::wstring explorer_args = L"/select,\"" + wide_path + L"\"";
     ShellExecuteW(nullptr, L"open", L"explorer.exe", explorer_args.c_str(), nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__)
     FSRef    file_ref;
     CFURLRef url =
       CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, (const UInt8 *)path.string().c_str(), path.string().size(), false);
     CFURLGetFSRef(url, &file_ref);
     LSOpenCFURLRef(&url, nullptr);
#elif defined(__linux__)
     // not thread safe
     std::string const folder_path = path.parent_path().string();
     std::string const cmd         = "xdg-open " + folder_path;
     int const         ret         = std::system(cmd.c_str());
     if (ret != 0)
     {
          spdlog::error("Command execution failed with error code {}. {}", ret, strerror(errno));
     }
#endif
}
