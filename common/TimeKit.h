/**
 * @project  BxScript (JS-like Scripting Language)
 * @author   BurNingLi
 * @date     2025/12/16
 * @license  MIT License
 *
 * @warning  USAGE DISCLAIMER / 免责声明
 * BxScript 仅供技术研究与合法开发。严禁用于灰产、黑客攻击等任何非法用途。
 * 开发者 BurNingLi 不承担因违规使用产生的任何法律责任。
 *
 * @brief    TimeKit
 */
#ifndef BXSCRIPT_TIMEKIT_H
#define BXSCRIPT_TIMEKIT_H
#include <chrono>
#include <filesystem>
#include <sstream>
#include <string>
namespace fs = std::filesystem;

class TimeKit {
public:
    static std::string GetFileLastTime(const fs::path &f) {
        const auto ftime = fs::last_write_time(f);
        const auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        const std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
        const std::tm *gmt = std::localtime(&tt);
        std::stringstream buffer;
        buffer << std::put_time(gmt, "%Y-%m-%d %H:%M:%S");
        return buffer.str();
    }
};


#endif //BXSCRIPT_TIMEKIT_H