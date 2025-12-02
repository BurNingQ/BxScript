#include <fstream>

#include "lexer/Lexer.h"
#include "iostream"
#include <windows.h>

std::string ReadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    Lexer lexer(R"(C:\Users\Administrator\Desktop\1.vbs)");

    while (!lexer.IsEndOfFile()) {
        std::cout << lexer.NextToken().ToString() << std::endl;
    }

    return 0;
}
