#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "parser/Parser.h"
#include "evaluator/Interpreter.h"
#include "evaluator/Environment.h"
#include "evaluator/Value.h"
#include "evaluator/EventLoop.h"
#include "gui/GuiRuntime.h"
#include "stdlib/GuiModule.h"

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

void SetupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

// 打印带颜色的 Prompt
void PrintPrompt() {
    std::cout << "\033[1;36m>> \033[0m"; // 青色加粗的 >>
    std::cout.flush();
}

// 打印结果 (绿色)
void PrintResult(const ValuePtr &res) {
    if (res->type != ValueType::NULL_TYPE) {
        std::cout << "\033[32m=> " << res->ToString() << "\033[0m" << std::endl;
    }
}

// 打印错误 (红色)
void PrintError(const std::string &msg) {
    std::cerr << "\033[31m[Error] " << msg << "\033[0m" << std::endl;
}

// 读取文件内容
std::string ReadFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ==========================================
// 模式 1: 交互式 REPL
// ==========================================
void RunRepl() {
    std::cout << "BxScript v1.0.0" << std::endl;
    std::cout << "Type 'exit' or 'quit' to leave." << std::endl;

    // 1. 创建持久化的全局环境
    auto env = std::make_shared<Environment>();
    Interpreter::SetupEnvironment(env);

    std::string line;
    while (true) {
        PrintPrompt();
        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line == "exit" || line == "quit") {
            break;
        }
        if (line.empty()) continue;

        try {
            // 3. 解析
            Parser parser(line);
            auto prog = std::make_shared<Program>(parser.ParseProgram());

            // 4. AST 保活 (放入注册表)
            Interpreter::ASTRegistry.push_back(prog);

            // 5. 执行 (使用持久化的 env)
            ValuePtr res = Interpreter::EvaluateProgram(*prog, env);

            // 6. 打印结果
            PrintResult(res);

            // 7. 顺便处理一下积压的异步任务
            EventLoop::Dispatch(0);
        } catch (const std::exception &e) {
            PrintError(e.what());
        }
    }
}

// ==========================================
// 模式 2: 文件执行
// ==========================================
void RunFile(const std::string &path) {
    try {
        // 1. 准备环境
        auto env = std::make_shared<Environment>();
        Interpreter::SetupEnvironment(env);

        // 2. 读取 & 解析
        std::string source = ReadFile(path);
        Parser parser(source);
        auto prog = std::make_shared<Program>(parser.ParseProgram());

        // 3. 保活
        Interpreter::ASTRegistry.push_back(prog);

        // 4. 执行
        Interpreter::EvaluateProgram(*prog, env);

        // 5. 进入事件循环保活 (CLI 模式核心)
        // 只有当有异步任务时，这里才会阻塞，否则直接退出
        EventLoop::RunLoop();
    } catch (const std::exception &e) {
        PrintError(e.what());
        exit(1);
    }
}

// ==========================================
// 入口 Main
// ==========================================
int main(int argc, char *argv[]) {
    SetupConsole();
    if (argc > 1) {
        RunFile(argv[1]);
    } else {
        RunRepl();
        return 0;
    }
    if (!GuiModule::GlobalForms.empty()) {
        GuiRuntime::Run();
    } else {
        EventLoop::RunLoop();
    }
    return 0;
}
