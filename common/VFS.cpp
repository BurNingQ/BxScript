/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2026/3/5
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    VFS
 */
#include "VFS.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else // Linux
#include <unistd.h>
#include <limits.h>
#endif

bool VFS::IsBundled = false;
fs::path VFS::DevRoot = fs::current_path();
mz_zip_archive VFS::ZipArchive = {};
std::vector<char> VFS::ZipData = {};

// 魔法常量
static auto MAGIC_TAG = "BX_PROJECT_ZIP";
static constexpr size_t MAGIC_LEN = 14;
static constexpr size_t SIZE_LEN = 4;

fs::path VFS::GetExecutablePath(const char *argv0) {
#ifdef _WIN32
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    return fs::path(buf);
#elif defined(__APPLE__)
    char buf[1024];
    uint32_t size = sizeof(buf);
    if (_NSGetExecutablePath(buf, &size) == 0) return fs::path(buf);
#elif defined(__linux__)
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf));
    if (len != -1) {
        buf[len] = '\0';
        return fs::path(buf);
    }
#endif
    return fs::absolute(fs::path(argv0));
}

void VFS::Mount(const char *argv0) {
    const fs::path exePath = GetExecutablePath(argv0);

    // 以二进制只读模式打开自身
    std::ifstream file(exePath, std::ios::binary | std::ios::ate);
    // 无法打开自己，默认为开发模式
    if (!file.is_open()) return;

    const std::streamsize fileSize = file.tellg();
    if (fileSize < static_cast<std::streamsize>(MAGIC_LEN + SIZE_LEN)) return;

    // 1. 校验魔法标记
    // 倒退 14 字节读取 Magic
    file.seekg(-static_cast<std::streamoff>(MAGIC_LEN), std::ios::end);
    char magicBuf[MAGIC_LEN + 1] = {0};
    file.read(magicBuf, MAGIC_LEN);

    if (std::strcmp(magicBuf, MAGIC_TAG) != 0) {
        // 没找到标记，说明是普通 EXE，处于开发模式
        return;
    }

    // 2. 获取 ZIP 大小
    // 倒退 18 字节 (14 Magic + 4 Size)
    file.seekg(-static_cast<std::streamoff>(MAGIC_LEN + SIZE_LEN), std::ios::end);
    uint32_t zipSize = 0;
    file.read(reinterpret_cast<char *>(&zipSize), SIZE_LEN);

    // *注意*：这里默认是小端序 (Little Endian)，Windows/Linux x86 都是小端，直接读没问题。
    // 如果需要严谨，可以在这里处理字节序转换。

    if (zipSize <= 0 || zipSize > fileSize) {
        std::cerr << "[VFS] Invalid ZIP size detected." << std::endl;
        return;
    }

    // 3. 读取 ZIP 数据到堆内存
    // 定位到 ZIP 开始的位置：文件末尾 - Magic - Size - ZipData
    const std::streamoff zipStartOffset = fileSize - MAGIC_LEN - SIZE_LEN - zipSize;
    file.seekg(zipStartOffset, std::ios::beg);

    ZipData.resize(zipSize);
    if (!file.read(ZipData.data(), zipSize)) {
        std::cerr << "[VFS] Failed to read ZIP data." << std::endl;
        return;
    }

    // 4. 初始化 miniz
    mz_zip_zero_struct(&ZipArchive);
    if (!mz_zip_reader_init_mem(&ZipArchive, ZipData.data(), zipSize, 0)) {
        std::cerr << "[VFS] Failed to initialize miniz reader." << std::endl;
        return;
    }

    IsBundled = true;
}

std::string VFS::ReadFile(const std::string &relPath) {
    if (IsBundled) {
        // ZIP 内部路径通常使用 '/'，为了兼容性，统一转一下斜杠
        std::string zipPath = relPath;
        std::replace(zipPath.begin(), zipPath.end(), '\\', '/');

        // 查询文件是否存在
        int fileIndex = mz_zip_reader_locate_file(&ZipArchive, zipPath.c_str(), nullptr, 0);
        if (fileIndex < 0) {
            throw std::runtime_error("File not found in bundle: " + relPath);
        }

        // 提取文件到堆
        size_t fileSize = 0;
        void *pData = mz_zip_reader_extract_file_to_heap(&ZipArchive, zipPath.c_str(), &fileSize, 0);
        if (!pData) {
            throw std::runtime_error("Failed to extract file from bundle: " + relPath);
        }

        // 构造 string 并释放 miniz 分配的内存
        std::string content(static_cast<char *>(pData), fileSize);
        mz_free(pData);
        return content;
    }
    // 开发模式：物理硬盘读取
    fs::path fullPath = DevRoot / relPath;
    // 二进制读取，避免 CRLF 转换问题影响 parse
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("File not found on disk: " + fullPath.string());
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
