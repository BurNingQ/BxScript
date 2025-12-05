//
// Created by BurNingLi on 2025/12/3.
//
#include <algorithm>

#include "../Value.h"
#include <cmath>
#include "../Logger.h"

ValuePtr ArrayValue::Get(const std::string &key) {
    try {
        const size_t index = std::stoul(key);
        if (index < Elements.size()) return Elements[index];
        return std::make_shared<NullValue>();
    } catch (...) {
        if (key == "length") return std::make_shared<NumberValue>(static_cast<double>(Elements.size()));
        if (key == "push") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())](
                const std::vector<ValuePtr> &args) -> ValuePtr {
                for (const auto &arg: args) self->Elements.push_back(arg);
                return std::make_shared<NumberValue>(static_cast<double>(self->Elements.size()));
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "pop") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())](
                const std::vector<ValuePtr> &args) -> ValuePtr {
                if (self->Elements.empty()) return std::make_shared<NullValue>();
                ValuePtr last = self->Elements.back();
                self->Elements.pop_back();
                return last;
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "shift") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())]
            (const std::vector<ValuePtr> &args) -> ValuePtr {
                if (self->Elements.empty()) return std::make_shared<NullValue>();
                auto v = self->Elements.front();
                self->Elements.erase(self->Elements.begin());
                return v;
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "unshift") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())]
            (const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<NumberValue>(self->Elements.size());
                }
                self->Elements.insert(self->Elements.begin(), args.begin(), args.end());
                return std::make_shared<NumberValue>(self->Elements.size());
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "concat") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())]
            (const std::vector<ValuePtr> &args) -> ValuePtr {
                std::vector<ValuePtr> newElements = self->Elements;
                for (const auto &arg: args) {
                    if (arg->type == ValueType::ARRAY) {
                        auto otherArr = std::static_pointer_cast<ArrayValue>(arg);
                        newElements.insert(newElements.end(), otherArr->Elements.begin(), otherArr->Elements.end());
                    } else {
                        newElements.push_back(arg);
                    }
                }
                return std::make_shared<ArrayValue>(std::move(newElements));
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "join") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())]
            (const std::vector<ValuePtr> &args) -> ValuePtr {
                std::string sep = ",";
                if (!args.empty()) {
                    sep = args[0]->ToString();
                }
                if (self->Elements.empty()) {
                    return std::make_shared<StringValue>("");
                }
                std::ostringstream ss;
                for (size_t i = 0; i < self->Elements.size(); ++i) {
                    ss << self->Elements[i]->ToString();
                    if (i < self->Elements.size() - 1) {
                        ss << sep;
                    }
                }
                return std::make_shared<StringValue>(ss.str());
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "remove") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())]
            (const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) {
                    Logger::Error("参数错误: removeAt(index, [count])");
                }
                if (args[0]->type != ValueType::NUMBER) {
                    Logger::Error("参数错误: 索引必须是数字");
                }
                const long index = static_cast<long>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                long count = 1;
                if (args.size() > 1) {
                    if (args[1]->type != ValueType::NUMBER) {
                        Logger::Error("参数错误: 数量必须是数字");
                    }
                    count = static_cast<long>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                }
                if (index < 0 || index >= self->Elements.size()) {
                    return std::make_shared<NullValue>();
                }
                if (count <= 0) {
                    return std::make_shared<NullValue>();
                }
                if (index + count > self->Elements.size()) {
                    count = self->Elements.size() - index;
                }
                std::vector<ValuePtr> removedItems;
                const auto startIt = self->Elements.begin() + index;
                const auto endIt = startIt + count;
                removedItems.assign(startIt, endIt);
                self->Elements.erase(startIt, endIt);
                return std::make_shared<ArrayValue>(removedItems);
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "insert") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())]
            (const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.size() < 2) {
                    Logger::Error("参数错误: insert(index, item)");
                }
                if (args[0]->type != ValueType::NUMBER) {
                    Logger::Error("参数错误: 索引必须是数字");
                }
                const long index = static_cast<long>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                if (index < 0 || index > self->Elements.size()) {
                    Logger::Error("参数错误: insert 索引越界");
                }
                self->Elements.insert(self->Elements.begin() + index, args[1]);
                return std::make_shared<NumberValue>(self->Elements.size());
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
        if (key == "slice") {
            auto fn = [self = std::static_pointer_cast<ArrayValue>(shared_from_this())]
            (const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<ArrayValue>(std::vector(self->Elements));
                }
                if (args[0]->type != ValueType::NUMBER) {
                    Logger::Error("参数错误: slice(number, [end])");
                }
                auto len = self->Elements.size();
                auto start = static_cast<long long>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                if (start < 0) {
                    start = len + start;
                    if (start < 0) {
                        start = 0;
                    }
                }
                if (start > len) {
                    return std::make_shared<ArrayValue>(std::vector<ValuePtr>());
                }
                auto begin = self->Elements.begin() + start;
                if (args.size() > 1) {
                    if (args[1]->type != ValueType::NUMBER) {
                        Logger::Error("参数错误: slice(number, [end])");
                    }
                    auto end = static_cast<long long>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                    if (end < 0) {
                        end = len + end;
                    }
                    if (start >= end) {
                        return std::make_shared<ArrayValue>(std::vector<ValuePtr>());
                    }
                    if (end > len) {
                        return std::make_shared<ArrayValue>(std::vector(begin, self->Elements.end()));
                    }
                    return std::make_shared<ArrayValue>(std::vector(begin, self->Elements.begin() + end));
                }
                return std::make_shared<ArrayValue>(std::vector(begin, self->Elements.end()));
            };
            return std::make_shared<NativeFunctionValue>(fn);
        }
    }
    return RuntimeValue::Get(key);
}

void ArrayValue::Set(const std::string &key, const ValuePtr value) {
    try {
        const size_t index = std::stoul(key);
        if (index >= Elements.size()) Elements.resize(index + 1, std::make_shared<NullValue>());
        Elements[index] = value;
    } catch (...) {
        Logger::Error("数组索引必须是整数: " + key);
    }
}

ValuePtr ArrayValue::InitBuiltins() {
    auto arrayObj = std::make_shared<ObjectValue>();
    arrayObj->Set("prototype", Prototype);
    auto isArrayFn = std::make_shared<NativeFunctionValue>(
        [](const std::vector<ValuePtr> &args) -> ValuePtr {
            if (args.empty()) {
                return std::make_shared<BoolValue>(false);
            }
            return std::make_shared<BoolValue>(args[0]->type == ValueType::ARRAY);
        });
    arrayObj->Set("isArray", isArrayFn);
    return arrayObj;
}
