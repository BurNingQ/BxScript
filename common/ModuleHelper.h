/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025-12-15
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    模块工具；路径转换，文件读取
 */

#ifndef BXSCRIPT_MODULEHELPER_H
#define BXSCRIPT_MODULEHELPER_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

class ModuleHelper {
public:
    static std::string ReadFile(const std::string &path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("无法打开模块文件: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    static fs::path GetExecutableDir() {
        return fs::current_path();
    }

    // 解析 import std.zz.dd -> 真实路径
    static std::string ResolvePath(const std::vector<std::string> &parts) {
        if (parts.empty()) return "";
        fs::path basePath;
        // std库
        if (parts[0] == "std") {
            basePath = GetExecutableDir() / "lib";
            for (size_t i = 1; i < parts.size(); ++i) {
                basePath /= parts[i];
            }
        } else {
            // 非标库
            basePath = fs::current_path();
            for (const auto &part: parts) {
                basePath /= part;
            }
        }
        basePath.replace_extension(".bx");
        if (!fs::exists(basePath)) {
            throw std::runtime_error("找不到模块文件: " + basePath.string());
        }
        return fs::absolute(basePath).string();
    }
};


#endif //BXSCRIPT_MODULEHELPER_H
