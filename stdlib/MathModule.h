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
 * @brief    数学标准库
 */

#ifndef BXSCRIPT_MATHMODULE_H
#define BXSCRIPT_MATHMODULE_H
#include <algorithm>
#include <cmath>
#include <random>

#include "evaluator/Value.h"


class MathModule {
    static std::mt19937 &GetRandomEngine() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }

    static void InitPI(std::shared_ptr<ObjectValue> &o) {
        o->Set("PI", std::make_shared<NumberValue>(3.141592653589793));
    }

    static void InitRound(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::round(v->Value));
            });
        o->Set("round", fn);
    }

    static void InitLog(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::log(v->Value));
            });
        o->Set("log", fn);
    }

    static void InitLog10(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::log10(v->Value));
            });
        o->Set("log10", fn);
    }

    static void InitLog1p(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::log1p(v->Value));
            });
        o->Set("log1p", fn);
    }

    static void InitLog2(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::log2(v->Value));
            });
        o->Set("log2", fn);
    }

    static void InitPow(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.size() != 2 || args[0]->type != ValueType::NUMBER || args[1]->type != ValueType::NUMBER) {
                    return std::make_shared<NullValue>();
                }
                auto v1 = std::static_pointer_cast<NumberValue>(args[0]);
                auto v2 = std::static_pointer_cast<NumberValue>(args[1]);
                return std::make_shared<NumberValue>(std::pow(v1->Value, v2->Value));
            });
        o->Set("pow", fn);
    }

    static void InitSqrt(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::sqrt(v->Value));
            });
        o->Set("sqrt", fn);
    }

    static void InitAbs(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::abs(v->Value));
            });
        o->Set("abs", fn);
    }

    static void InitCeil(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::ceil(v->Value));
            });
        o->Set("ceil", fn);
    }

    static void InitFloor(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::floor(v->Value));
            });
        o->Set("floor", fn);
    }

    static void InitCbrt(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::cbrt(v->Value));
            });
        o->Set("cbrt", fn);
    }

    static void InitSin(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::sin(v->Value));
            });
        o->Set("sin", fn);
    }

    static void InitSinh(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::sinh(v->Value));
            });
        o->Set("sinh", fn);
    }

    static void InitASin(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::asin(v->Value));
            });
        o->Set("asin", fn);
    }

    static void InitASinh(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::asinh(v->Value));
            });
        o->Set("asinh", fn);
    }

    static void InitCos(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::cos(v->Value));
            });
        o->Set("cos", fn);
    }

    static void InitCosh(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::cosh(v->Value));
            });
        o->Set("cosh", fn);
    }

    static void InitACos(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::acos(v->Value));
            });
        o->Set("acos", fn);
    }

    static void InitACosh(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::acosh(v->Value));
            });
        o->Set("acosh", fn);
    }

    static void InitTan(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::tan(v->Value));
            });
        o->Set("tan", fn);
    }

    static void InitTanh(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::tanh(v->Value));
            });
        o->Set("tanh", fn);
    }

    static void InitATan(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::atan(v->Value));
            });
        o->Set("atan", fn);
    }

    static void InitATanh(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::atanh(v->Value));
            });
        o->Set("atanh", fn);
    }

    static void InitExp(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::exp(v->Value));
            });
        o->Set("exp", fn);
    }

    static void InitExpm1(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::expm1(v->Value));
            });
        o->Set("expm1", fn);
    }

    static void InitTrunc(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty() || args[0]->type != ValueType::NUMBER) return std::make_shared<NullValue>();
                auto v = std::static_pointer_cast<NumberValue>(args[0]);
                return std::make_shared<NumberValue>(std::trunc(v->Value));
            });
        o->Set("trunc", fn);
    }

    static void InitMax(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<NullValue>();
                }
                double maxValue = -std::numeric_limits<double>::infinity();;
                for (auto &v: args) {
                    if (v->type == ValueType::NUMBER) {
                        auto arg = std::static_pointer_cast<NumberValue>(v)->Value;
                        if (arg > maxValue) {
                            maxValue = arg;
                        }
                    }
                }
                return std::make_shared<NumberValue>(maxValue);
            });
        o->Set("max", fn);
    }

    static void InitMin(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                if (args.empty()) {
                    return std::make_shared<NullValue>();
                }
                double minValue = std::numeric_limits<double>::infinity();
                for (auto &v: args) {
                    if (v->type == ValueType::NUMBER) {
                        auto arg = std::static_pointer_cast<NumberValue>(v)->Value;
                        if (arg < minValue) {
                            minValue = arg;
                        }
                    }
                }
                return std::make_shared<NumberValue>(minValue);
            });
        o->Set("min", fn);
    }

    static void InitRandom(std::shared_ptr<ObjectValue> &o) {
        const auto fn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args)-> ValuePtr {
                auto &gen = GetRandomEngine();
                if (args.size() == 2 && args[0]->type == ValueType::NUMBER && args[1]->type == ValueType::NUMBER) {
                    long min = static_cast<long>(std::static_pointer_cast<NumberValue>(args[0])->Value);
                    long max = static_cast<long>(std::static_pointer_cast<NumberValue>(args[1])->Value);
                    if (min > max) std::swap(min, max);
                    std::uniform_int_distribution<> dis(min, max);
                    return std::make_shared<NumberValue>(dis(gen));
                }
                std::uniform_real_distribution<> dis(0.0, 1.0);
                return std::make_shared<NumberValue>(dis(gen));
            });
        o->Set("random", fn);
    }

public:
    static ValuePtr CreateNetModule() {
        auto module = std::make_shared<ObjectValue>();
        InitPI(module);
        InitRound(module);
        InitLog(module);
        InitLog10(module);
        InitLog1p(module);
        InitLog2(module);
        InitPow(module);
        InitSqrt(module);
        InitAbs(module);
        InitCeil(module);
        InitFloor(module);
        InitCbrt(module);
        InitSin(module);
        InitSinh(module);
        InitASin(module);
        InitASinh(module);
        InitCos(module);
        InitCosh(module);
        InitACos(module);
        InitACosh(module);
        InitTan(module);
        InitTanh(module);
        InitATan(module);
        InitATanh(module);
        InitExp(module);
        InitExpm1(module);
        InitTrunc(module);
        InitMax(module);
        InitMin(module);
        InitRandom(module);
        return module;
    }
};


#endif //BXSCRIPT_MATHMODULE_H
