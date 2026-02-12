#include "resource_path.h"
#include <filesystem>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#include <climits>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace graphgl {

static std::string getExecutableDir() {
#ifdef __APPLE__
    char buf[1024];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) == 0) {
        return std::filesystem::path(buf).parent_path().string();
    }
#elif defined(__linux__)
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len != -1) {
        buf[len] = '\0';
        return std::filesystem::path(buf).parent_path().string();
    }
#elif defined(_WIN32)
    char buf[MAX_PATH];
    if (GetModuleFileNameA(nullptr, buf, MAX_PATH) != 0) {
        return std::filesystem::path(buf).parent_path().string();
    }
#endif
    return "";
}

std::string resolveResourcePath(const std::string& relativePath) {
    std::string exeDir = getExecutableDir();
    if (!exeDir.empty()) {
        std::string resolved = exeDir + "/" + relativePath;
        if (std::filesystem::exists(resolved)) {
            return resolved;
        }
    }
    // Fall back to CWD-relative path.
    return relativePath;
}

} // namespace graphgl
