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
 * @brief    日志
 */

#ifndef BXSCRIPT_LOGGER_H
#define BXSCRIPT_LOGGER_H
#include <iostream>
#include <sstream>
#include <string>


class Logger {
public:
    static void Error(const std::string &message) {
        std::ostringstream oss;
        oss << "\033[1;31m[运行错误]\033[0m " << message;
        std::cerr << oss.str() << std::endl;
        throw std::runtime_error(message);
    }

};


#endif //BXSCRIPT_LOGGER_H