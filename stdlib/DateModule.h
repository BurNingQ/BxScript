//
// Created by BurNingLi on 2025/12/9.
//

#ifndef BXSCRIPT_DATEMODULE_H
#define BXSCRIPT_DATEMODULE_H

#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include "../evaluator/Value.h"
#include "common/StringKit.h"

using namespace std::chrono;

class DateModule {
    static double GetCurrentMillis() {
        const auto now = system_clock::now();
        const auto duration = now.time_since_epoch();
        return static_cast<double>(duration_cast<milliseconds>(duration).count());
    }

public:
    static ValuePtr CreateDateObject() {
        auto dateObj = std::make_shared<ObjectValue>();
        dateObj->Set("now", std::make_shared<NativeFunctionValue>(
                         [](const std::vector<ValuePtr> &args) -> ValuePtr {
                             return std::make_shared<NumberValue>(GetCurrentMillis());
                         }
                     ));
        dateObj->Set("sleep", std::make_shared<NativeFunctionValue>(
                         [](const std::vector<ValuePtr> &args) -> ValuePtr {
                             if (!args.empty() && args[0]->type == ValueType::NUMBER) {
                                 const long ms = static_cast<long>(std::static_pointer_cast<NumberValue>(args[0])->
                                     Value);
                                 std::this_thread::sleep_for(milliseconds(ms));
                             }
                             return std::make_shared<NullValue>();
                         }
                     ));
        dateObj->Set("format", std::make_shared<NativeFunctionValue>(
                         [](const std::vector<ValuePtr> &args) -> ValuePtr {
                             if (args.empty()) return std::make_shared<NullValue>();
                             const double ms = std::static_pointer_cast<NumberValue>(args[0])->Value;
                             const auto tt = static_cast<time_t>(ms / 1000.0);
                             std::string fmt = "yyyy-MM-dd HH:mm:ss";
                             if (args.size() >= 2 && args[1]->type == ValueType::STRING) {
                                 fmt = std::static_pointer_cast<StringValue>(args[1])->Value;
                             }
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
                     ));
        return dateObj;
    }
};


#endif //BXSCRIPT_DATEMODULE_H
