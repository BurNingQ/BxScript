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
#ifndef BXSCRIPT_VFS_H
#define BXSCRIPT_VFS_H
#include <string>
#include <vector>
#include <filesystem>
#include "libs/zip/miniz.h"


namespace fs = std::filesystem;

class VFS {
public:
    static bool IsBundled; // 是否为打包模式
    static fs::path DevRoot; // 开发模式下的根目录

    /**
     * 挂载虚拟文件系统
     * @param argv0 main 函数传入的 argv[0]，用于跨平台获取执行路径的兜底方案
     */
    static void Mount(const char *argv0);

    /**
     * 读取文件内容 (自动判断是从 ZIP 还是 磁盘)
     * @param relPath 相对路径 (例如 "src/main.bx" 或 "config.json")
     * @return 文件内容字符串
     * @throws std::runtime_error 如果文件读取失败
     */
    static std::string ReadFile(const std::string &relPath);

private:
    static mz_zip_archive ZipArchive; // miniz 归档句柄
    static std::vector<char> ZipData; // 驻留内存的 ZIP 二进制数据

    // 跨平台获取当前可执行文件路径
    static fs::path GetExecutablePath(const char *argv0);
};


#endif //BXSCRIPT_VFS_H
