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
#include "common/VFS.h"
#include "libs/json.hpp"

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

// ==========================================
// 模式 1: 交互式 REPL
// ==========================================
void RunRepl() {
    std::cout << "BxScript v1.0.0 (REPL)" << std::endl;
    std::cout << "Type 'exit' or 'quit' to leave." << std::endl;
    const auto env = std::make_shared<Environment>();
    Interpreter::SetupEnvironment(env);
    std::string line;
    while (true) {
        PrintPrompt();
        if (!std::getline(std::cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;
        try {
            Parser parser(line);
            auto prog = std::make_shared<Program>(parser.ParseProgram());
            Interpreter::ASTRegistry.push_back(prog);
            ValuePtr res = Interpreter::EvaluateProgram(*prog, env);
            PrintResult(res);
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
        const std::string source = VFS::ReadFile(path);
        Interpreter::Run(source);
    } catch (const std::exception &e) {
        const std::string msg = "Execution Failed [" + path + "]: " + e.what();
        PrintError(msg);
#ifdef _WIN32
        CommonDlgs::MsgBoxError(nullptr, StringKit::U8ToU16(msg), L"BxScript Runtime");
#endif
        exit(1);
    }
}

std::string ReadIndexFile() {
    try {
        const std::string source = VFS::ReadFile("config.json");
        if (auto j = nlohmann::json::parse(source); j.is_object()) {
            if (j.contains("project_main") && j["project_main"].is_string()) {
                return j["project_main"].get<std::string>();
            }
        } else {
#ifdef _WIN32
            CommonDlgs::MsgBoxError(nullptr, L"清单文件丢失", L"BxScript Runtime");
#endif
        }
        return "src/index.bx";
    } catch (std::exception &e) {
#ifdef _WIN32
        CommonDlgs::MsgBoxError(nullptr, L"清单文件丢失", L"BxScript Runtime");
#endif
        exit(1);
    }
}

// ==========================================
// 入口 Main
// ==========================================
int main(const int argc, char *argv[]) {
    SetupConsole();
    VFS::Mount(argv[0]);
    // 打包模式
    if (VFS::IsBundled) {
        try {
            RunFile(ReadIndexFile());
        } catch (std::exception &e) {
            std::string msg = "包加载失败: ";
            msg += e.what();
            PrintError(msg);
            return 1;
        }
    } else {
        if (argc > 1) {
            const fs::path scriptPath(argv[1]);
            VFS::DevRoot = fs::absolute(scriptPath).parent_path();
            RunFile(argv[1]);
        } else {
            RunRepl();
            return 0;
        }
    }
    if (!GuiModule::GlobalForms.empty()) {
        GuiRuntime::Run(GuiModule::GlobalForms);
    } else {
        EventLoop::RunLoop();
    }
    return 0;
}
