//
// Created by BurNingLi on 2025/11/25.
//

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