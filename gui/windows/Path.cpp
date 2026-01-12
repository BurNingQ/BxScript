/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/1/12
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    Path
 */
#include "Path.h"
#include <windows.h>
#include <shlobj.h>
#include <filesystem>
#include "internal/Shell32.h"
#include "internal/Kernel32.h"

namespace fs = std::filesystem;

std::wstring Path::knownFolderPath(int id) {
    wchar_t buf[MAX_PATH];

    if (!Shell32::W32_SHGetSpecialFolderPath(nullptr, buf, id, FALSE)) {
        return L"";
    }

    return std::wstring(buf);
}

std::wstring Path::AppDataPath() {
    return knownFolderPath(CSIDL_APPDATA);
}

std::wstring Path::CommonAppDataPath() {
    return knownFolderPath(CSIDL_COMMON_APPDATA);
}

std::wstring Path::LocalAppDataPath() {
    return knownFolderPath(CSIDL_LOCAL_APPDATA);
}

std::wstring Path::EnsureAppDataPath(const std::wstring &company, const std::wstring &product) {
    std::wstring path = AppDataPath();
    if (path.empty()) {
        return L"";
    }

    fs::path p(path);
    p /= company;
    p /= product;

    if (!fs::exists(p)) {
        std::error_code ec;
        if (!fs::create_directories(p, ec)) {
            return p.wstring();
        }
    }
    return p.wstring();
}

std::vector<std::wstring> Path::DriveNames() {
    unsigned int bufLen = Kernel32::W32_GetLogicalDriveStrings(0, nullptr);
    if (bufLen == 0) {
        return {};
    }

    std::vector<wchar_t> buf(bufLen + 1);
    bufLen = Kernel32::W32_GetLogicalDriveStrings(bufLen + 1, buf.data());
    if (bufLen == 0) {
        return {};
    }

    std::vector<std::wstring> names;
    const wchar_t *p = buf.data();
    while (*p) {
        std::wstring name(p);
        names.push_back(name);
        p += name.length() + 1;
    }

    return names;
}
