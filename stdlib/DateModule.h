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
 * @brief    日期标准库
 */

#ifndef BXSCRIPT_DATEMODULE_H
#define BXSCRIPT_DATEMODULE_H

#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include "../evaluator/Value.h"
#include "common/StringKit.h"
#include "evaluator/Logger.h"
#include <iomanip>

using namespace std::chrono;

class DateModule {
    static double GetCurrentMillis() {
        const auto now = system_clock::now();
        const auto duration = now.time_since_epoch();
        return static_cast<double>(duration_cast<milliseconds>(duration).count());
    }

    static void FillDateObject(std::shared_ptr<ObjectValue> o, double timestamp, const tm *ptm) {
        o->Set("timestamp", std::make_shared<NumberValue>(timestamp));
        o->Set("year", std::make_shared<NumberValue>(static_cast<double>(ptm->tm_year + 1900)));
        o->Set("month", std::make_shared<NumberValue>(static_cast<double>(ptm->tm_mon + 1)));
        o->Set("day", std::make_shared<NumberValue>(static_cast<double>(ptm->tm_mday)));
        o->Set("hour", std::make_shared<NumberValue>(static_cast<double>(ptm->tm_hour)));
        o->Set("minute", std::make_shared<NumberValue>(static_cast<double>(ptm->tm_min)));
        o->Set("second", std::make_shared<NumberValue>(static_cast<double>(ptm->tm_sec)));
    }

    static void SkipSeparators(std::istream &is) {
        while (is.peek() != EOF && !isdigit(is.peek())) {
            is.get();
        }
    }

    static void initNow(std::shared_ptr<ObjectValue> &o) {
        const auto nowFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                auto o = std::make_shared<ObjectValue>();
                const auto timestamp = GetCurrentMillis();
                const auto tt = static_cast<time_t>(timestamp / 1000.0);
                const tm *ptm = localtime(&tt);
                FillDateObject(o, timestamp, ptm);
                initFormat(o);
                return o;
            }
        );
        o->Set("now", nowFn);
    }

    static void initFrom(std::shared_ptr<ObjectValue> &o) {
        const auto fromFn = std::make_shared<NativeFunctionValue>(
            [](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty() || (args[0]->type != ValueType::NUMBER && args[0]->type != ValueType::STRING)) {
                    Logger::Error("参数错误: Date.from(timestamp/dateString)");
                }
                auto o = std::make_shared<ObjectValue>();
                long long timestamp = 0;
                if (args[0]->type == ValueType::NUMBER) {
                    const auto arg = std::static_pointer_cast<NumberValue>(args[0]);
                    if (arg->Value > 0) {
                        timestamp = static_cast<long long>(arg->Value);
                    }
                } else if (args[0]->type == ValueType::STRING) {
                    const auto arg = std::static_pointer_cast<StringValue>(args[0]);
                    std::tm tm = {};
                    std::istringstream ss{arg->Value};
                    ss >> tm.tm_year;
                    SkipSeparators(ss);
                    ss >> tm.tm_mon;
                    SkipSeparators(ss);
                    ss >> tm.tm_mday;
                    SkipSeparators(ss);
                    ss >> tm.tm_hour;
                    SkipSeparators(ss);
                    ss >> tm.tm_min;
                    SkipSeparators(ss);
                    ss >> tm.tm_sec;
                    if (tm.tm_year > 0) {
                        tm.tm_year -= 1900;
                        tm.tm_mon -= 1;
                        timestamp = std::mktime(&tm) * 1000;
                    } else {
                        Logger::Error("参数错误: Date.from(yyyy-MM-dd HH:mm:ss/yyyy年MM月dd日 HH时mm分:ss秒)");
                    }
                }
                const auto tt = static_cast<time_t>(static_cast<double>(timestamp) / 1000.0);
                const tm *ptm = localtime(&tt);
                FillDateObject(o, static_cast<double>(timestamp), ptm);
                initFormat(o);
                return o;
            });
        o->Set("from", fromFn);
    }

    static void initFormat(std::shared_ptr<ObjectValue> &o) {
        std::weak_ptr weak_o = o;
        const auto formatFn = std::make_shared<NativeFunctionValue>(
            [weak_o](const std::vector<ValuePtr> &args) -> ValuePtr {
                if (args.empty()) return std::make_shared<NullValue>();
                const auto self = weak_o.lock();
                std::string fmt = "yyyy-MM-dd HH:mm:ss";
                if (args[0]->type != ValueType::STRING) {
                    Logger::Error("参数错误: Date.format(fmt)");
                } else {
                    fmt = std::static_pointer_cast<StringValue>(args[0])->Value;
                }
                const auto timestamp = self->Get("timestamp");
                // 理论上只有可能为NULL或者NUMBER
                if (timestamp->type == ValueType::NULL_TYPE) {
                    return std::make_shared<NullValue>();
                }
                if (timestamp->type != ValueType::NUMBER) {
                    Logger::Error("参数错误: 内部timestamp类型错误");
                }
                const double ms = std::static_pointer_cast<NumberValue>(timestamp)->Value;
                const auto tt = static_cast<time_t>(ms / 1000.0);
                fmt = StringKit::ReplaceAll(fmt, "yyyy", "%Y");
                fmt = StringKit::ReplaceAll(fmt, "MM", "%m");
                fmt = StringKit::ReplaceAll(fmt, "dd", "%d");
                fmt = StringKit::ReplaceAll(fmt, "HH", "%H");
                fmt = StringKit::ReplaceAll(fmt, "hh", "%I");
                fmt = StringKit::ReplaceAll(fmt, "mm", "%M");
                fmt = StringKit::ReplaceAll(fmt, "ss", "%S");
                const tm *ptm = localtime(&tt);
                char buffer[128];
                strftime(buffer, 128, fmt.c_str(), ptm);
                return std::make_shared<StringValue>(buffer);
            }
        );
        o->Set("format", formatFn);
    }

public:
    static ValuePtr CreateDateModule() {
        auto dateObj = std::make_shared<ObjectValue>();
        initNow(dateObj);
        initFrom(dateObj);
        return dateObj;
    }
};


#endif //BXSCRIPT_DATEMODULE_H
