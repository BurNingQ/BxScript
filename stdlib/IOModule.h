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
 * @brief    IO标准库
 */

#ifndef BXSCRIPT_FILEMODULE_H
#define BXSCRIPT_FILEMODULE_H
#include <iostream>

#include "common/TimeKit.h"
#include "evaluator/Value.h"

namespace fs = std::filesystem;

class IOModule {
    static void InitPrintln(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                std::string output;
                for (const auto &v: args) {
                    if (!output.empty()) {
                        output += ", ";
                    }
                    output += v->ToString();
                }
                std::cout << output << std::endl;
                return std::make_shared<NullValue>();
            });
        o->Set("println", fn);
    }

    static void InitPrint(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                std::string output;
                for (const auto &v: args) {
                    if (!output.empty()) {
                        output += ", ";
                    }
                    output += v->ToString();
                }
                std::cout << output;
                return std::make_shared<NullValue>();
            });
        o->Set("print", fn);
    }

    static void InitInput(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (!args.empty()) {
                    std::cout << args[0]->ToString();
                }
                std::string input;
                std::getline(std::cin, input);
                return std::make_shared<StringValue>(input);
            });
        o->Set("input", fn);
    }

    static void InitExist(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<BoolValue>(false);
                }
                return std::make_shared<BoolValue>(fs::exists(args[0]->ToString()));
            });
        o->Set("exist", fn);
    }

    static void InitIsFile(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<BoolValue>(false);
                }
                return std::make_shared<BoolValue>(fs::is_regular_file(args[0]->ToString()));
            });
        o->Set("isFile", fn);
    }

    static void InitIsDir(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<BoolValue>(false);
                }
                return std::make_shared<BoolValue>(fs::is_directory(args[0]->ToString()));
            });
        o->Set("isDir", fn);
    }

    static void InitMkdir(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<BoolValue>(false);
                }
                return std::make_shared<BoolValue>(fs::create_directories(args[0]->ToString()));
            });
        o->Set("mkdir", fn);
    }

    static void InitRemove(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<BoolValue>(false);
                }
                return std::make_shared<BoolValue>(fs::remove_all(args[0]->ToString()));
            });
        o->Set("remove", fn);
    }

    static void InitCopyFile(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.size() != 2) {
                    return std::make_shared<BoolValue>(false);
                }
                if (args[0]->type != ValueType::STRING || args[1]->type != ValueType::STRING) {
                    return std::make_shared<BoolValue>(false);
                }
                fs::copy(args[0]->ToString(), args[1]->ToString());
                return std::make_shared<BoolValue>(true);
            });
        o->Set("copy", fn);
    }

    static void InitRename(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<BoolValue>(false);
                }
                if (args[0]->type != ValueType::STRING || args[1]->type != ValueType::STRING) {
                    return std::make_shared<BoolValue>(false);
                }
                fs::rename(args[0]->ToString(), args[1]->ToString());
                return std::make_shared<BoolValue>(true);
            });
        o->Set("rename", fn);
    }

    static void InitList(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<ArrayValue>(std::vector<ValuePtr>());
                }
                if (args[0]->type != ValueType::STRING) {
                    return std::make_shared<ArrayValue>(std::vector<ValuePtr>());
                }
                auto arrs = std::make_shared<ArrayValue>(std::vector<ValuePtr>());
                for (auto &f: fs::directory_iterator(args[0]->ToString())) {
                    auto o = std::make_shared<ObjectValue>();
                    if (f.is_directory()) {
                        o->Set("type", std::make_shared<StringValue>("dir"));
                        o->Set("size", std::make_shared<NumberValue>(0));
                    } else {
                        o->Set("type", std::make_shared<StringValue>("file"));
                        double size = 0;
                        if (f.is_regular_file()) {
                            size = static_cast<double>(f.file_size());
                        }
                        o->Set("size", std::make_shared<NumberValue>(size));
                    }
                    o->Set("lastModified", std::make_shared<StringValue>(TimeKit::GetFileLastTime(f)));
                    arrs->Elements.push_back(o);
                }
                return std::move(arrs);
            });
        o->Set("list", fn);
    }

    static void InitAbs(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::STRING) {
                    return std::make_shared<StringValue>("");
                }
                auto p = fs::absolute(args[0]->ToString());
                return std::make_shared<StringValue>(p.string());
            });
        o->Set("abs", fn);
    }

    static void InitAttr(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::STRING) {
                    return std::make_shared<ObjectValue>();
                }
                auto o = std::make_shared<ObjectValue>();
                auto f = fs::path(args[0]->ToString());
                if (exists(f)) {
                    o->Set("size", std::make_shared<NumberValue>(file_size(f)));
                    o->Set("name", std::make_shared<StringValue>(f.filename().string()));
                    o->Set("size", std::make_shared<NumberValue>(0));
                    if (fs::is_regular_file(f)) {
                        o->Set("size", std::make_shared<NumberValue>(static_cast<double>(fs::file_size(f))));
                    }
                }
                return o;
            });
        o->Set("attr", fn);
    }

    static void InitRead(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) return std::make_shared<NullValue>();
                std::string path = args[0]->ToString();
                std::ifstream file(path, std::ios::binary | std::ios::ate);
                if (!file.is_open()) return std::make_shared<NullValue>();
                const std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                std::vector<unsigned char> buffer(size);
                if (file.read((char *) buffer.data(), size)) {
                    if (args.size() > 1 && args[1]->ToString() == "utf8") {
                        std::string str(buffer.begin(), buffer.end());
                        return std::make_shared<StringValue>(str);
                    }
                    return std::make_shared<BufferValue>(std::move(buffer));
                }
                return std::make_shared<NullValue>();
            });
        o->Set("read", fn);
    }

    static void InitWrite(std::shared_ptr<ObjectValue> &o) {
        auto const fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.size() < 2) return std::make_shared<BoolValue>(false);
                std::string path = args[0]->ToString();
                auto data = args[1];
                std::ofstream file(path, std::ios::binary);
                if (!file.is_open()) return std::make_shared<BoolValue>(false);
                if (data->type == ValueType::STRING) {
                    std::string str = std::static_pointer_cast<StringValue>(data)->Value;
                    file.write(str.c_str(), str.size());
                } else if (data->type == ValueType::BUFFER) {
                    auto buf = std::static_pointer_cast<BufferValue>(data);
                    file.write((char *) buf->Buffer.data(), buf->Buffer.size());
                }
                return std::make_shared<BoolValue>(true);
            });
        o->Set("write", fn);
    }

public:
    static ValuePtr CreateIOModule() {
        auto module = std::make_shared<ObjectValue>();
        InitPrintln(module);
        InitPrint(module);
        InitInput(module);
        InitExist(module);
        InitIsFile(module);
        InitIsDir(module);
        InitMkdir(module);
        InitRemove(module);
        InitCopyFile(module);
        InitRename(module);
        InitList(module);
        InitAbs(module);
        InitAttr(module);
        InitRead(module);
        InitWrite(module);
        return module;
    }
};


#endif //BXSCRIPT_FILEMODULE_H
